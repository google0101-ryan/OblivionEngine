#pragma once

#include <string>
#include <vector>

class IResource;
class IRenderImage;

// Represents a single pass on the scene
// Each step of the rendering process (g-buffers, transparency, depth pass, shadows, etc etc) all get their own pass
class IRenderPass
{
public:
    virtual ~IRenderPass() {}
    // Attach a specific shader to this pass (REQUIRED)
    virtual void AddShader(std::string name) = 0;
    // Actually build the render pass
    // Takes a render target and all inputs the pass has (i.e. all textures and buffers)
    virtual void Create(std::vector<IRenderImage*> pOutputImages, bool isOutputPass, std::vector<IResource*> pInputs, std::vector<IResource*> pOutputs, bool enableDepthTest) = 0;
    // Commit current uniforms, images, etc to memory
    virtual void CommitAll() = 0;
    // Set image for index in shader
    virtual void SetImageAtIndex(int index, IRenderImage* pImage) = 0;
    // End frame
    virtual void EndFrame() = 0;
};

// Define this as a seperate class so we can do fancier things like render graphs later
class RenderPassManager
{
public:
    IRenderPass* CreateRenderPass();
};

extern RenderPassManager* g_pRenderPassManager;