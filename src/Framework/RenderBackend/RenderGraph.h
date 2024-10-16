#pragma once

#include <Framework/RenderBackend/RenderGraphResource.h>
#include <Framework/RenderBackend/RenderPassManager.h>
#include <vector>
#include <unordered_map>
#include <functional>

class RenderGraph
{
public:
    void DoFrame();
    void EndFrame();

    void AttachResource(IResource* pResource);

    IRenderPass* MakePass(std::string name, bool isOutputPass, bool enableDepthTest, std::vector<std::string> renderTargets, std::vector<std::string> inputs, std::vector<std::string> outputs, std::function<void()> frameCB);
private:
    // List of all potential inputs for each pass
    // This allows render passes to reference dependencies by name only
    std::unordered_map<std::string, IResource*> m_Resources;

    struct PassInfo
    {
        IRenderPass* m_pPass;
        std::function<void()> m_Callback;
    };

    std::unordered_map<std::string, PassInfo> m_Passes;
};

extern RenderGraph* g_pRenderGraph;