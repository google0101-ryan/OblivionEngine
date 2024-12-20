include_directories(${CMAKE_SOURCE_DIR}/public)

option(COMPILE_VULKAN "Enables vulkan support for Oblivion Engine" ON)

# Needed because Windows and DLLs are a headache
# Otherwise we'd need a way to copy SDL3.dll into both the root dir and the bin/ folder
# Just statically link it instead
set(SDL_STATIC ON CACHE INTERNAL "")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/SDL EXCLUDE_FROM_ALL)

if (NOT COMPILE_VULKAN)
    add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/glew/build/cmake EXCLUDE_FROM_ALL)
    include_directories(${CMAKE_SOURCE_DIR}/vendor/glew/include)
endif()

# find_package(SDL3 REQUIRED)

set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)
set(CMAKE_CXX_STANDARD 20)
include_directories(${CMAKE_SOURCE_DIR}/vendor/SDL/include)

if (COMPILE_VULKAN)
    add_compile_definitions(VULKAN_RENDERER)
endif()

set(LAYER0_HEADERS public/layer0/layer0.h
                   public/layer0/Lexer.h
                   public/layer0/Token.h)

set(LAYER0_SOURCES private/layer0/Lexer.cpp)

add_library(layer0 STATIC ${LAYER0_SOURCES} ${LAYER0_HEADERS})

set(APPSYSTEM_SOURCES private/appsystem/AppSystem.cpp
                     private/appsystem/AppSystemGroup.cpp)

set(APPSYSTEM_HEADERS public/appsystem/AppSystem.h
                        public/appsystem/AppSystemGroup.h)

add_library(appsystem STATIC ${APPSYSTEM_SOURCES} ${APPSYSTEM_HEADERS})

set(LAYER1_SOURCES private/layer1/Cvar.cpp
                   private/layer1/Command.cpp
                   private/layer1/EventPump.cpp)

set(LAYER1_HEADERS public/layer1/Cvar.h
                    public/layer1/Command.h
                    public/layer1/layer1.h
                    public/layer1/EventPump.h)

add_library(layer1 SHARED ${LAYER1_SOURCES} ${LAYER1_HEADERS})
target_link_libraries(layer1 appsystem layer0)

set(LAYER2_SOURCES private/layer2/Window.cpp
                    private/layer2/FileSystem.cpp)

set(LAYER2_HEADERS public/layer2/Window.h
                    public/layer2/FileSystem.h
                    public/layer2/layer2.h
                    private/layer2/File.h)

add_library(layer2 SHARED ${LAYER2_SOURCES} ${LAYER2_HEADERS})
target_link_libraries(layer2 appsystem layer0 SDL3::SDL3-static)
if (COMPILE_VULKAN)
    target_link_libraries(layer2 vulkan)
endif()

set(LAYER3_HEADERS public/layer3/layer3.h
                   public/layer3/renderer/Renderer.h
                   private/layer3/renderer/Backend.h)

if (COMPILE_VULKAN)

    set(LAYER3_SOURCES private/layer3/renderer/Renderer.cpp
                       private/layer3/renderer/vkbackend/Backend.cpp
                       private/layer3/renderer/vkbackend/vulkan/VkInstance.cpp
                       private/layer3/renderer/vkbackend/vulkan/VkDevice.cpp
                       private/layer3/renderer/vkbackend/vulkan/VkSwapchain.cpp
                       private/layer3/renderer/vkbackend/vulkan/VkProgram.cpp
                       private/layer3/renderer/vkbackend/vulkan/VkShader.cpp
                       private/layer3/renderer/vkbackend/vulkan/VkCommandBuffer.cpp
                       private/layer3/renderer/vkbackend/vulkan/VkInstance.h
                       private/layer3/renderer/vkbackend/vulkan/VkDevice.h
                       private/layer3/renderer/vkbackend/vulkan/VkSwapchain.h
                       private/layer3/renderer/vkbackend/vulkan/VkProgram.h
                       private/layer3/renderer/vkbackend/vulkan/VkShader.h
                       private/layer3/renderer/vkbackend/vulkan/VkCommandBuffer.h)

else()

    set(LAYER3_SOURCES private/layer3/renderer/Renderer.cpp
                       private/layer3/renderer/glbackend/Backend.cpp)

endif()

add_library(layer3 SHARED ${LAYER3_HEADERS} ${LAYER3_SOURCES})
if (COMPILE_VULKAN)
    target_link_libraries(layer3 vulkan appsystem)
else()
    target_link_libraries(layer3 glew_s appsystem)
endif()

set(ENGINE_SOURCES private/app/App.cpp
                    private/engine/main/EngineApp.cpp
                    private/engine/main/main.cpp)

set(ENGINE_HEADERS public/app/App.h
                    public/engine/main/EngineApp.h)

add_executable(engine ${ENGINE_SOURCES} ${ENGINE_HEADERS})
target_link_libraries(engine appsystem)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT engine)

install(TARGETS engine DESTINATION .)
install(TARGETS layer1 DESTINATION bin)
install(TARGETS layer2 DESTINATION bin)
install(TARGETS layer3 DESTINATION bin)