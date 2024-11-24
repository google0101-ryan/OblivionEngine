#pragma once

#include <layer2/layer2.h>
#include <layer2/FileSystem.h>
#include <layer2/Window.h>

template<typename T>
class CLayer3AppSystem : public CLayer2AppSystem<T>
{
    DECLARE_APPSYSTEM( CLayer2AppSystem<T>, CLayer3AppSystem )
public:
    virtual void Connect( SystemFactoryFn_t pFunc )
    {
        m_pWindow = static_cast<IWindow*>(pFunc(WINDOW_INTERFACE_NAME));
        m_pFilesystem = static_cast<IFileSystem*>(pFunc(FILESYSTEM_INTERFACE_NAME));

        BaseClass::Connect( pFunc );
    }

    IWindow* GetWindow()
    {
        return m_pWindow;
    }

    IFileSystem* GetFilesystem()
    {
        return m_pFilesystem;
    }
private:
    IWindow* m_pWindow;
    IFileSystem* m_pFilesystem;
};