#pragma once

#include <layer0/layer0.h>

typedef void* (*CreateSystemFactory_t)(const char*);

abstract_class IAppSystem
{
public:
    // When this is called, the app can query for any systems it wants to
    // have references to
    virtual void Connect( CreateSystemFactory_t pFactory ) = 0;

    virtual bool Create() = 0;
    virtual void Destroy() = 0;
};

template<class TInterface>
class CBaseAppSystem : public TInterface
{
public:
    virtual void Connect( CreateSystemFactory_t pFactory ) {}
};

template<class TInterface>
class CLayer1AppSystem : public CBaseAppSystem<TInterface>
{
};