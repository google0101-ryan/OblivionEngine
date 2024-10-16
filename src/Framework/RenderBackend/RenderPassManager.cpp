#include <Framework/RenderBackend/RenderPassManager.h>

RenderPassManager g_localRenderPassManager;
RenderPassManager* g_pRenderPassManager = &g_localRenderPassManager;