#include <layer3/renderer/Renderer.h>
#include <layer3/layer3.h>
#include "Backend.h"

class CRenderer : public CLayer3AppSystem<IRenderer>
{
public:
    CRenderer()
    {
        m_pBackend = CreateBackend();
    }

    virtual bool Init();
    virtual void Shutdown();

    virtual void Connect(SystemFactoryFn_t pFunc)
    {
        CLayer3AppSystem<IRenderer>::Connect(pFunc);
        m_pBackend->Connect(pFunc);
    }
private:
    IBackend* m_pBackend;
};

CRenderer g_renderer;
EXPOSE_INTERFACE_GLOBALVAR(CRenderer, IRenderer, g_renderer, RENDERER_INTERFACE_NAME);

bool CRenderer::Init()
{
    printf("----- Renderer Init -----\n");

    m_pBackend->SubmitCommand(BC_INIT, nullptr);

    printf("-------------------------\n");
    return true;
}

void CRenderer::Shutdown()
{
    m_pBackend->SubmitCommand(BC_SHUTDOWN, nullptr);
}
