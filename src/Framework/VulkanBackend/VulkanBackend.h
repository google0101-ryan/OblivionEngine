#pragma once

#include <Framework/RenderBackend/RenderBackend.h>


class VulkanBackend : public IRenderBackend
{
protected:
    virtual void BeginFrame();
    virtual void Init(RenderInitArgs* args);
    virtual void DrawFrame(void*);
    virtual void SubmitGeometry(Geometry* geo);
    virtual void PlatShutdown();
private:
    bool m_bInitialized = false;
};