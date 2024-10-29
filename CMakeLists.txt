include_directories(${CMAKE_SOURCE_DIR}/public)

set(CXX_LAYER0_SOURCES public/layer0/String.h
                       public/layer0/Platform.h
                       public/layer0/Linux/LinuxPlatform.h
                       private/layer0/String.cpp)

add_library(layer0 STATIC ${CXX_LAYER0_SOURCES})

set(CXX_APPSYSTEM_SOURCES public/appsystem/AppSystem.h
                          public/appsystem/interface.h
                          private/appsystem/interface.cpp)

add_library(appsystem STATIC ${CXX_APPSYSTEM_SOURCES})

set(CXX_LAUNCHER_SOURCES private/launcher/launcher.cpp)

add_executable(launcher ${CXX_LAUNCHER_SOURCES})
target_link_libraries(launcher layer0)

set(CXX_ENGINE_SOURCES public/app/App.h
                       public/engine/Version.h
                       public/engine/main/EngineApp.h
                       private/app/App.cpp
                       private/engine/EngineMain.cpp
                       private/engine/main/EngineApp.cpp)

add_library(engine SHARED ${CXX_ENGINE_SOURCES})

set(CXX_LAYER1_SOURCES public/layer1/Command.h
                       public/layer1/EventSystem.h
                       private/layer1/Command.cpp
                       private/layer1/EventSystem.cpp)

add_library(layer1 SHARED ${CXX_LAYER1_SOURCES})
target_link_libraries(layer1 appsystem)

install(TARGETS launcher DESTINATION .)
install(TARGETS engine DESTINATION bin)
install(TARGETS layer1 DESTINATION bin)