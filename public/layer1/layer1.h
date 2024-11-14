#pragma once

#include <appsystem/AppSystem.h>

template<typename BaseClass>
class CLayer1AppSystem : public BaseClass
{
public:
    // this just stubs out connect because layer1 systems should be dependency-free ideally
    virtual void Connect(SystemFactoryFn_t pFactory) {}
};