#include <Framework/GameWorld/GameWorld.h>
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
    auto modelsToSubmit = g_GameWorld->GetModels();
    for (auto& model : modelsToSubmit)
    {
        m_pBackend->SendCommand(RENDER_CMD_SUBMIT_MESH, model);
    }

    m_pBackend->SendCommand(RENDER_CMD_DRAWFRAME, nullptr);

    m_pBackend->WaitFrame();
}