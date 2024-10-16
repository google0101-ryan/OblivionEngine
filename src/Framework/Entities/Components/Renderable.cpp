#include <Framework/GameWorld/GameWorld.h>
#include <Framework/Entities/Components/Renderable.h>
#include <Framework/RenderFrontend/RenderFrontend.h>

Renderable::Renderable()
{
    m_Model = new Model(); // Creates a default model and slaps a missing texture on it
}

void Renderable::Update()
{
    g_GameWorld->AddGeometry(m_Model);
}