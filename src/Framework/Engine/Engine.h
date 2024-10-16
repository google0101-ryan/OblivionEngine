#pragma once

#include <Framework/Window/Window.h>
#include <Framework/Events/EventManager.h>
#include <Framework/RenderFrontend/RenderFrontend.h>
#include <functional>

using TickListener = std::function<void()>;

struct EngineConfig
{
    int iWidth = 1024;
    int iHeight = 768;
    const char* pszTitle = "Application";
    bool bFullscreen = false;
    int iArgc = 0;
    const char** ppArgv = nullptr;
};

class Engine
{
public:
    /**
     * @brief Initialize core engine systems
     */
    void Init(EngineConfig& config);
    /**
     * @brief Called once per frame to update core engine components
     */
    void Tick();

    /**
     * @brief Returns whether or not the engine is running
     */
    bool IsRunning();
    /**
     * @brief Stop the engine from running after the current frame is completed
     */
    void RequestShutdown();
    /**
     * @brief Engine cleanup. Will exit the application.
     */
    void Exit(int iCode);

    void AddTickListener(TickListener listener);
    void AddFixedTickListener(TickListener listener);

    // Engine subsystem access
    EventManager* GetEventManager();
    Window* GetMainWindow();
private:
    std::vector<TickListener> tickListeners;
    std::vector<TickListener> fixedTickListeners;
};

extern Engine* g_pEngine;