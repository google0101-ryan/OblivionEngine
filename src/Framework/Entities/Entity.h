#pragma once

#include <unordered_map>
#include <string>

// Empty class for components to derive from
class IEntityComponent
{
public:
    virtual ~IEntityComponent() {}
};

// An entity is basically a container of components, which implement unique functionality
// There are two types of entities: programatic and editor-based
// Programatic entities are defined in their own c++ files, and are used for game-specific functionality
// These are similar to entities in Valve or Quake editors
// Editor-based are defined in the editor, with various components attached manually by the developer
// They offer less functionality than programatic entities, as they cannot be programmed
class Entity
{
public:
    virtual ~Entity() {}

    IEntityComponent* GetComponentByName(std::string& name);
    IEntityComponent* GetComponentByName(const char* name);
private:
    std::unordered_map<std::string, IEntityComponent*> m_Components;
};