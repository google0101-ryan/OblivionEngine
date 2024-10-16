#include "Entity.h"

IEntityComponent *Entity::GetComponentByName(std::string &name)
{
    return m_Components[name];
}

IEntityComponent *Entity::GetComponentByName(const char *name)
{
    return m_Components[name];
}