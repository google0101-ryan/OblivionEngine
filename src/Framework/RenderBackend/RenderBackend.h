#pragma once

#include <Framework/Entities/Components/Camera.h>
#include <Framework/Platform/Platform.h>
#include <Framework/RenderBackend/Geometry.h>

#include <Cstdlib/Queue.h>

enum RenderCmd
{
    RENDER_CMD_INVALID = -1,
    RENDER_CMD_INIT,
    RENDER_CMD_BEGIN_FRAME,
    RENDER_CMD_SUBMIT_MESH,
    RENDER_CMD_DRAWFRAME,
    RENDER_CMD_SET_CAMERA,
};

struct RenderInitArgs
{
};

class IRenderBackend : public SysThread
{
public:
    void SendCommand(int iCommand, void* pData);

    void WaitFrame()
    {
        std::unique_lock lock(m_BackendDoneMutex);
        m_BackendDoneCond.wait(lock);
    }
protected:
    virtual void DoWork();
    virtual void Shutdown();
protected:
    struct RenderCmdData
    {
        int m_iCmd = RENDER_CMD_INVALID;
        void* m_pData;
    };

    CLib::Queue<RenderCmdData> m_RenderCmdQ;

    std::condition_variable m_BackendDoneCond;
    std::mutex m_BackendDoneMutex;
protected:
    virtual void Init(RenderInitArgs* pArgs) = 0;
    virtual void BeginFrame() = 0;
    virtual void DrawFrame(void*) = 0;
    virtual void SubmitGeometry(Model* geo) = 0;
    virtual void SetCamera(Camera* pCamera) = 0;
    virtual void PlatShutdown() = 0;
};

// Implemented in each backend (Vulkan, OpenGL, etc.)
IRenderBackend* CreateBackend();