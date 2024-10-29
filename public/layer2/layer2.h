#pragma once

#include <appsystem/AppSystem.h>

void ConnectLayer2Systems( CreateSystemFactory_t pFactory );

template<typename TInterface>
class CLayer2AppSystem : public CLayer1AppSystem<TInterface>
{
    typedef CLayer1AppSystem<TInterface> BaseClass;
public:
    virtual void Connect( CreateSystemFactory_t pFactory )
    {
        BaseClass::Connect( pFactory );

        ConnectLayer2Systems( pFactory );
    }
};