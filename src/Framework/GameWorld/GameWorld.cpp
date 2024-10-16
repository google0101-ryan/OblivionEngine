#pragma once

#include <Framework/GameWorld/GameWorld.h>

GameWorld localGameWorld;
GameWorld* g_GameWorld = &localGameWorld;

void GameWorld::AddGeometry(Model* model)
{
    m_Geometry.push_back(model);
}