#pragma once

#include <string>

enum ResourceType
{
    RT_IMAGE, // specifically samplers
    RT_BUFFER, // misc. buffers
};

class IResource
{
public:
    IResource(ResourceType rType, std::string name)
    {
        m_Type = rType;
        m_Name = name;
    }

    const std::string& GetName() const {return m_Name;}
protected:
    ResourceType m_Type;
    std::string m_Name;
};