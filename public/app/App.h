#pragma once

#include <layer0/layer0.h>

abstract_class IApplication
{
public:
    virtual bool Create() = 0;
    virtual void Main() = 0;
    virtual void Shutdown() = 0;
};

class CBasicApp : public IApplication
{
public:
    bool Run();
protected:
    bool m_bIsRunning;
};