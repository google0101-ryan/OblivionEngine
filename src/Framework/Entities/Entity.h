#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <vector>

class Entity;

// Empty class for components to derive from
class IEntityComponent
{
public:
    virtual ~IEntityComponent() {}

    void SetParent(Entity* parent)
    {
        m_pParent = parent;
    }

    // Called once per frame by Entity::Update()
    virtual void Update() = 0;
protected:
    Entity* m_pParent;
};

// An entity is basically a container of components, which implement unique functionality
// There are two types of entities: programatic and editor-based
// Programatic entities are defined in their own c++ files, and are used for game-specific functionality
// These are similar to entities in Valve or Quake editors
// Editor-based are defined in the editor, with various components attached manually by the developer
// They offer less functionality than programatic entities, as they cannot be programmed
// These are like your generic Unity modules if they couldn't have C# scripts attached
class Entity
{
public:
    Entity(const char* name);
    virtual ~Entity() {}

    virtual void Update()
    {
        for (auto& component : m_Components)
            component.second->Update();
    }

    IEntityComponent* GetComponentByName(std::string& name);
    IEntityComponent* GetComponentByName(const char* name);

    void AddComponent(std::string& name, IEntityComponent* component)
    {
        component->SetParent(this);
        m_Components[name] = component;
    }
    void AddComponent(const char* name, IEntityComponent* component)
    {
        component->SetParent(this);
        m_Components[name] = component;
    }

    glm::vec3 pos, rot, scale; // These are on every entity
protected:
    typedef Entity BaseClass;
private:
    std::unordered_map<std::string, IEntityComponent*> m_Components;
    
};

class EntityManager
{
public:
    void Init();
    void AddEntity(Entity* pEntity);

    void UpdateAll();
private:
    std::vector<Entity*> m_pEntities;
};

extern EntityManager* g_EntityManager;