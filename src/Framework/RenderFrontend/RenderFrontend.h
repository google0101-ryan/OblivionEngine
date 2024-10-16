#pragma once

class IRenderBackend;

class RenderFrontend
{
public:
    void Init();
    void Exit();

    void BeginFrame();
    void DrawWorld();
private:
    IRenderBackend* m_pBackend;
};