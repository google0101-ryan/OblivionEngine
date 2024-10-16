#pragma once

// Represents every mesh, entity, light, etc. that make up the game world

#include <Framework/Entities/Components/Camera.h>
#include <Framework/RenderBackend/Geometry.h>

#include <vector>
class GameWorld
{
public:
    void AddGeometry(Model* model);
    void SetCamera(Camera* pCamera);

    const std::vector<Model*>& GetModels() const { return m_Geometry; }
private:
    std::vector<Model*> m_Geometry;
    Camera* m_pActiveCamera;
};

extern GameWorld* g_GameWorld;