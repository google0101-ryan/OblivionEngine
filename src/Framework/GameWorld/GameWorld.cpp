#pragma once

#include <Framework/GameWorld/GameWorld.h>

GameWorld localGameWorld;
GameWorld* g_GameWorld = &localGameWorld;

void GameWorld::AddGeometry(Model* model)
{
    // TODO: Do things like frustum culling here?
    m_Geometry.push_back(model);
}

void GameWorld::SetCamera(Camera* pCamera)
{
    m_pActiveCamera = pCamera;
}