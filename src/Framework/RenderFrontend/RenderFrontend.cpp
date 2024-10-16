#include <Framework/RenderFrontend/RenderFrontend.h>

#include <Framework/RenderBackend/RenderBackend.h>
#include <Framework/RenderBackend/Geometry.h>
#include <Framework/RenderBackend/Geometry.h>

void RenderFrontend::Init()
{
    m_pBackend = CreateBackend();
    m_pBackend->Start();

    RenderInitArgs args;
    m_pBackend->SendCommand(RENDER_CMD_INIT, &args);
}

void RenderFrontend::Exit()
{
    m_pBackend->Stop();
}

void RenderFrontend::BeginFrame()
{
    m_pBackend->SendCommand(RENDER_CMD_BEGIN_FRAME, nullptr);
}

void RenderFrontend::DrawWorld()
{
    if (defaultModel)
    {
        m_pBackend->SendCommand(RENDER_CMD_SUBMIT_MESH, &rectangle);
        m_pBackend->SendCommand(RENDER_CMD_SUBMIT_MESH, defaultModel->GetGeo());
    }

    m_pBackend->SendCommand(RENDER_CMD_DRAWFRAME, nullptr);

    m_pBackend->WaitFrame();
}