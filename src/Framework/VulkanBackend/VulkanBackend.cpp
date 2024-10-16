#include "Framework/Filesystem/FileSystem.h"
#include "Framework/Platform/Linux/Linux.inl"
#include "Framework/RenderBackend/RenderFormat.h"
#include "Framework/VulkanBackend/Vulkan/Image.h"
#include <Framework/VulkanBackend/VulkanBackend.h>

#include <Framework/Engine/Engine.h>
#include <Framework/VulkanBackend/Vulkan/VkGodStruct.h>
#include <Framework/VulkanBackend/Vulkan/StagingBuffer.h>
#include <Framework/RenderBackend/RenderPassManager.h>
#include <Framework/RenderBackend/RenderGraphImage.h>
#include <Framework/RenderBackend/RenderGraph.h>

#include <Util/Logger.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <unistd.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan_core.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Framework/VulkanBackend/Vulkan/stb_image.h>

IRenderImage* CreateImage(std::string path)
{
    auto file = g_pFilesystem->OpenFileRead(("textures/" + path).c_str());
    file->Seek(SEEK_END, 0);
    size_t size = file->Tell();
    file->Seek(SEEK_SET, 0);
    std::vector<stbi_uc> buffer(size+1);
    file->Read(size, buffer.data());

    int x, y;
    int channels;
    auto img = stbi_load_from_memory(buffer.data(), size, &x, &y, &channels, 4);

    IRenderImage* ret = new IRenderImage(path, false);
    VulkanImage* pImage = new VulkanImage(x, y, ColorFormat::CF_RGBA8_SRGB, ImageUsage::IU_REGULAR_IMAGE, VK_IMAGE_ASPECT_COLOR_BIT, true);
    pImage->CopyToImage(img, x*y*4);
    ret->SetAPIData(pImage);

    return ret;
}

VkState g_VkState;

const char* g_pszLayerNames[] =
{
    "VK_LAYER_KHRONOS_validation"
};

int g_pszLayerSize = sizeof(g_pszLayerNames) / sizeof(g_pszLayerNames[0]);

IRenderPass* examplePass;
std::vector<Model*> backendGeo;

UBO ubo;

void VulkanBackend::BeginFrame()
{
    backendGeo.clear();
    g_vertexStagingManager.BeginFrame();
    g_indexStagingManager.BeginFrame();
}

void VulkanBackend::Init(RenderInitArgs *args)
{
    Logger::Log(LogLevel::INFO, "Backend init");

    // We begin by initializing our window's Vulkan support
    g_pEngine->GetMainWindow()->InitVulkan();

    {
        uint32_t extensionCount;
        std::vector<const char*> extensions;
        g_pEngine->GetMainWindow()->GetInstanceExtensions(&extensionCount, NULL);
        extensions.resize(extensionCount);
        g_pEngine->GetMainWindow()->GetInstanceExtensions(&extensionCount, extensions.data());

        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        auto instance = 
        VulkanInstanceBuilder(g_pEngine->GetMainWindow()->GetInitialTitle(), VK_API_VERSION_1_3)
            .SetExtensions(extensions.size(), extensions.data())
#ifndef NDEBUG
            .SetLayers(g_pszLayerSize, g_pszLayerNames)
#endif
            .Build();
        
        g_VkState.m_Instance = new VulkanInstance(instance);
    }

    g_VkState.m_Surface = (VkSurfaceKHR)g_pEngine->GetMainWindow()
                                            ->GetSurface((void*)g_VkState.m_Instance->GetHandle());

    // We've got our instance. Now it's time to find the GPU to use
    {
        auto device =
        VulkanDeviceBuilder()
#ifndef NDEBUG
            .SetLayers(g_pszLayerSize, g_pszLayerNames)
#endif
            .Build(g_VkState.m_Instance);
        
        g_VkState.m_Device = device;
    }

    auto formats = g_VkState.m_Device->GetFormats(g_VkState.m_Surface);

    if (formats.size() <= 0)
        Logger::Log(LogLevel::FATAL, "Failed to find valid display format");

    // Just select the first format for now
    g_VkState.m_Format = formats[0];

    // Select present mode
    g_VkState.m_PresentMode = g_VkState.m_Device->PickPresentMode(g_VkState.m_Surface, SwapChainPresentMode::PRESENT_MODE_FIFO);
    Logger::Log(LogLevel::INFO, "Picked swap mode %d", g_VkState.m_PresentMode);

    // TODO: Support high DPI displays
    g_VkState.m_iSwapChainWidth = g_pEngine->GetMainWindow()->GetWidth();
    g_VkState.m_iSwapChainHeight = g_pEngine->GetMainWindow()->GetHeight();

    g_VkState.m_SwapChain = new VulkanSwapChain();
    g_VkState.m_SwapChain->GetSwapChainImages(g_VkState.m_SwapChainImages);

    // For each image in the swap chain, create a new resource and attach the image
    for (size_t i = 0; i < g_VkState.m_SwapChainImages.size(); i++)
    {
        auto res = new IRenderImage("swap" + std::to_string(i), false);
        res->SetAPIData(reinterpret_cast<IImageData*>(&g_VkState.m_SwapChainImages[i]));
        g_pRenderGraph->AttachResource(res);
    }

    VkSemaphoreCreateInfo semaInfo = {};
    semaInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < MAX_FRAMES; i++)
    {
        g_VkState.m_FrameCommandBuffer[i].Create();

        vkCreateSemaphore(g_VkState.m_Device->GetDeviceHandle(), &semaInfo, nullptr, &g_VkState.m_imageSema[i]);
        vkCreateSemaphore(g_VkState.m_Device->GetDeviceHandle(), &semaInfo, nullptr, &g_VkState.m_renderSema[i]);

        vkCreateFence(g_VkState.m_Device->GetDeviceHandle(), &fenceInfo, nullptr, &g_VkState.m_inFlightFence[i]);
    }

    g_vertexStagingManager.Create();
    g_indexStagingManager.Create();

    g_VkState.m_VertexBuffer.Create(32*1024*1024, 
                                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                    false, false);
    
    g_VkState.m_IndexBuffer.Create(32*1024*1024,
                                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                    false, false);

    for (int i = 0; i < MAX_FRAMES; i++)
    {
        g_VkState.m_UniformBuffers[i].Create(sizeof(UBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, true, true);
    }

    g_VkState.depthBuffer = new VulkanImage(g_VkState.m_iSwapChainWidth, g_VkState.m_iSwapChainHeight, ColorFormat::CF_D24_S8, IU_DEPTH_ATTACHMENT, VK_IMAGE_ASPECT_DEPTH_BIT, false);
    auto depthRes = new IRenderImage("_depth");
    depthRes->SetAPIData(g_VkState.depthBuffer);
    g_pRenderGraph->AttachResource(depthRes);

    // Add a fallback option if the user has not specified a camera
    ubo.projection = glm::perspective(glm::radians(90.0f), (float)g_VkState.m_iSwapChainWidth / (float)g_VkState.m_iSwapChainHeight, 0.1f, 1000.0f);
    ubo.projection[1][1] *= -1;
    ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), 
  		   glm::vec3(0.0f, 0.0f, 0.0f), 
  		   glm::vec3(0.0f, 1.0f, 0.0f));

    if (m_pCamera)
    {
        ubo.projection = m_pCamera->GetProjection();
        ubo.projection[1][1] *= -1;
        ubo.view = m_pCamera->GetView();
    }

    // Now we get into some more user-friendly API: render pipeline creation
    // TODO: Switch to a graph-based renderer so we can do dependency ordering, multithreaded RenderPass optimization, etc
    examplePass = g_pRenderGraph->MakePass("example", true, true, {"swap0"}, {}, {"_depth"}, []()
    {
        g_VkState.m_FrameCommandBuffer[g_VkState.m_iFrameIndex].BeginRenderPass(examplePass, false);
        g_VkState.m_FrameCommandBuffer[g_VkState.m_iFrameIndex].BindPipeline(examplePass);

        memcpy(g_VkState.m_UniformBuffers[g_VkState.m_iFrameIndex].GetMappedData(), &ubo, sizeof(ubo));
        if (backendGeo.size())
        {
            for (auto& geo : backendGeo)
            {
                examplePass->SetImageAtIndex(0, geo->GetGeo()->m_Texture);
                examplePass->CommitAll();
                g_VkState.m_FrameCommandBuffer[g_VkState.m_iFrameIndex].BindDescriptors((VulkanRenderPass*)examplePass, false, 0);
                g_VkState.m_FrameCommandBuffer[g_VkState.m_iFrameIndex].PushConstants((VulkanRenderPass*)examplePass, sizeof(glm::mat4), &geo->GetModel(), VK_SHADER_STAGE_VERTEX_BIT);
                g_VkState.m_FrameCommandBuffer[g_VkState.m_iFrameIndex].BindVertexBuffer(g_VkState.m_VertexBuffer.GetBuffer(), g_vertexStagingManager.GetInfo(geo->GetGeo()->vertCacheHandle).m_iOffset);
                g_VkState.m_FrameCommandBuffer[g_VkState.m_iFrameIndex].BindIndexBuffer(g_VkState.m_IndexBuffer.GetBuffer(), g_indexStagingManager.GetInfo(geo->GetGeo()->indexCacheHandle).m_iOffset);
                g_VkState.m_FrameCommandBuffer[g_VkState.m_iFrameIndex].DrawIndexed(geo->GetGeo()->numIndices, 1, 0, 0, 0);
            }
        }

        g_VkState.m_FrameCommandBuffer[g_VkState.m_iFrameIndex].EndRenderPass();
    });

    m_bInitialized = true;
}

void VulkanBackend::DrawFrame(void *)
{
    if (!m_bInitialized || backendGeo.empty())
    {
        // Make sure nothing's waiting on us
        m_BackendDoneCond.notify_all();
        return;
    }

    uint64_t frameStartMs = Sys_Milliseconds();

    uint32_t imageIndex;
    if (auto result = vkAcquireNextImageKHR(g_VkState.m_Device->GetDeviceHandle(), g_VkState.m_SwapChain->GetSwapChain(), UINT64_MAX, g_VkState.m_imageSema[g_VkState.m_iFrameIndex], VK_NULL_HANDLE, &imageIndex); result != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to acquire next image with code = 0x%08x", result);

    vkResetFences(g_VkState.m_Device->GetDeviceHandle(), 1, &g_VkState.m_inFlightFence[g_VkState.m_iFrameIndex]);

    g_VkState.m_FrameCommandBuffer[g_VkState.m_iFrameIndex].Reset();

    // Copy geometry to the GPU so everything is where it's expected to be
    g_vertexStagingManager.Flush(g_VkState.m_VertexBuffer.GetBuffer());
    g_indexStagingManager.Flush(g_VkState.m_IndexBuffer.GetBuffer());
    
    g_VkState.m_FrameCommandBuffer[g_VkState.m_iFrameIndex].Begin();

    // Record commands from each pass
    g_pRenderGraph->DoFrame();
    
    g_VkState.m_FrameCommandBuffer[g_VkState.m_iFrameIndex].TransitionImageLayout(g_VkState.m_SwapChainImages[imageIndex].GetImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    g_VkState.m_FrameCommandBuffer[g_VkState.m_iFrameIndex].End();

    WaitStages wait[] = {WAIT_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    g_VkState.m_FrameCommandBuffer[g_VkState.m_iFrameIndex].Submit(1, &g_VkState.m_imageSema[g_VkState.m_iFrameIndex], 1, &g_VkState.m_renderSema[g_VkState.m_iFrameIndex], wait, g_VkState.m_inFlightFence[g_VkState.m_iFrameIndex]);

    vkQueueWaitIdle(g_VkState.m_Device->GetGraphicsQueue());

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &g_VkState.m_renderSema[g_VkState.m_iFrameIndex];
    VkSwapchainKHR swapChains[] = {g_VkState.m_SwapChain->GetSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    vkWaitForFences(g_VkState.m_Device->GetDeviceHandle(), 1, &g_VkState.m_inFlightFence[g_VkState.m_iFrameIndex], VK_TRUE, UINT64_MAX);

    if (vkQueuePresentKHR(g_VkState.m_Device->GetPresentQueue(), &presentInfo) != VK_SUCCESS)
        Logger::Log(LogLevel::FATAL, "Failed to present frame");

    g_VkState.m_iFrameIndex = (g_VkState.m_iFrameIndex + 1) % MAX_FRAMES;
    g_VkState.m_iFrame++;
    
    g_pRenderGraph->EndFrame();

    uint64_t frameTimeMs = Sys_Milliseconds() - frameStartMs;
//    printf("Frame time: %03ldms (%ld)\n", frameTimeMs, g_VkState.m_iFrame);
    
    m_BackendDoneCond.notify_all();
    g_pRenderGraph->EndFrame();
}

void VulkanBackend::SubmitGeometry(Model *geo)
{
    backendGeo.push_back(geo);
    geo->GetGeo()->vertCacheHandle = g_vertexStagingManager.AllocCache(geo->GetGeo()->verts, sizeof(DrawVert)*geo->GetGeo()->numVerts);
    geo->GetGeo()->indexCacheHandle = g_indexStagingManager.AllocCache(geo->GetGeo()->indices, geo->GetGeo()->numIndices*sizeof(uint16_t));
}

void VulkanBackend::PlatShutdown()
{
    WaitFrame();
    g_VkState.m_Device->WaitIdle();

    delete examplePass;

    g_VkState.m_VertexBuffer.Destroy();
    g_VkState.m_IndexBuffer.Destroy();

    g_vertexStagingManager.Destroy();
    g_indexStagingManager.Destroy();

    for (int i = 0; i < MAX_FRAMES; i++)
    {
        vkDestroySemaphore(g_VkState.m_Device->GetDeviceHandle(), g_VkState.m_imageSema[i], nullptr);
        vkDestroySemaphore(g_VkState.m_Device->GetDeviceHandle(), g_VkState.m_renderSema[i], nullptr);
        vkDestroyFence(g_VkState.m_Device->GetDeviceHandle(), g_VkState.m_inFlightFence[i], nullptr);
        g_VkState.m_FrameCommandBuffer[i].Destroy();
    }

    g_VkState.m_SwapChain->Destroy();
    delete g_VkState.m_SwapChain;

    g_VkState.m_Device->Destroy();
    delete g_VkState.m_Device;

    vkDestroySurfaceKHR(g_VkState.m_Instance->GetHandle(), g_VkState.m_Surface, nullptr);

    g_VkState.m_Instance->Destroy();
    delete g_VkState.m_Instance;

    m_bInitialized = false;
}

void VulkanBackend::SetCamera(Camera* pCamera)
{
    m_pCamera = pCamera;
}

IRenderBackend* CreateBackend()
{
    return new VulkanBackend;
}