#include <Framework/RenderBackend/RenderGraph.h>
#include <Framework/RenderBackend/RenderPassManager.h>

RenderGraph g_localRenderGraph;
RenderGraph* g_pRenderGraph = &g_localRenderGraph;

void RenderGraph::DoFrame()
{
    // TODO: Sort these based on inputs and outputs
    for (auto& pass : m_Passes)
    {
        pass.second.m_Callback();
    }
}

void RenderGraph::EndFrame()
{
    for (auto& pass : m_Passes)
    {
        pass.second.m_pPass->EndFrame();
    }
}

void RenderGraph::AttachResource(IResource *pResource)
{
    m_Resources[pResource->GetName()] = pResource;
}

IRenderPass *RenderGraph::MakePass(std::string name, bool isOutputPass, bool enableDepthTest, std::vector<std::string> renderTargets, std::vector<std::string> inputs, std::vector<std::string> outputs, std::function<void()> frameCB)
{
    std::vector<IRenderImage*> actualRenderTargets;
    actualRenderTargets.reserve(renderTargets.size());
    for (auto& target : renderTargets)
    {
        if (!m_Resources[target])
            return nullptr;
        actualRenderTargets.push_back(reinterpret_cast<IRenderImage*>(m_Resources[target]));
    }
    
    std::vector<IResource*> actualInputs;
    actualInputs.reserve(inputs.size());
    for (auto& input : inputs)
    {
        if (!m_Resources[input])
            return nullptr;
        actualInputs.push_back(m_Resources[input]);
    }
    
    std::vector<IResource*> actualOutputs;
    actualOutputs.reserve(outputs.size());
    for (auto& output : outputs)
    {
        if (!m_Resources[output])
            return nullptr;
        actualOutputs.push_back(m_Resources[output]);
    }

    IRenderPass* pass = g_pRenderPassManager->CreateRenderPass();
    pass->AddShader(name);
    pass->Create(actualRenderTargets, isOutputPass, actualInputs, actualOutputs, enableDepthTest);

    PassInfo info;
    info.m_pPass = pass;
    info.m_Callback = frameCB;
    m_Passes[name] = info;

    return pass;
}
