#include "Entity.h"
#include "Framework/Engine/Engine.h"

Entity::Entity(const char* name)
{
    pos = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    rot = glm::vec3(0.0f);

    g_EntityManager->AddEntity(this);
}

IEntityComponent *Entity::GetComponentByName(std::string &name)
{
    return m_Components[name];
}

IEntityComponent *Entity::GetComponentByName(const char *name)
{
    return m_Components[name];
}

EntityManager entityManagerLocal;
EntityManager* g_EntityManager = &entityManagerLocal;

void EntityManager::Init()
{
    g_pEngine->AddTickListener(std::bind(&EntityManager::UpdateAll, this));
}

void EntityManager::AddEntity(Entity* pEntity)
{
    m_pEntities.push_back(pEntity);
}

void EntityManager::UpdateAll()
{
    for (auto& entity : m_pEntities)
        entity->Update();
}