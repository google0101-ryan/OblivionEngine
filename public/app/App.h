#pragma once

#include <layer0/layer0.h>

abstract_class IApplication
{
public:
    virtual void Init() = 0;
    virtual void Main() = 0;
    virtual void Shutdown() = 0;
};

// Adds some convenience stuff like m_isRunning
class CBaseApplication : public IApplication
{
public:
    void Run();
protected:
    bool m_isRunning;
};