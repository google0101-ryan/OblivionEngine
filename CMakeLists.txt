project(OBLIVION)

set(CMAKE_MINIMUM_REQUIRED_VERSION 3.16.3)

set(CXX_STANDARD c++23)

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

file(GLOB SHADERS
    ${CMAKE_SOURCE_DIR}/base/shaderprogs/*.vert
    ${CMAKE_SOURCE_DIR}/base/shaderprogs/*.frag)

add_custom_command(
    COMMAND
    ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/base/shaderbin
    OUTPUT ${CMAKE_SOURCE_DIR}/base/shaderbin
    COMMENT "Creating base/shaderbin"
)

foreach(source IN LISTS SHADERS)
    get_filename_component(FILENAME ${source} NAME)
    add_custom_command(
        COMMAND
        ${glslc_executable} -o ${CMAKE_SOURCE_DIR}/base/shaderbin/${FILENAME}.spv ${source}
        OUTPUT ${CMAKE_SOURCE_DIR}/base/shaderbin/${FILENAME}.spv
        DEPENDS ${source} ${CMAKE_SOURCE_DIR}/base/shaderbin
        COMMENT "Compiling ${FILENAME}"
    )
    list(APPEND SPV_SHADERS ${CMAKE_SOURCE_DIR}/base/shaderbin/${FILENAME}.spv)
endforeach()

add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})

set(SOURCES src/Util/Logger.cpp
            src/Cstdlib/Lexer.cpp
            src/Framework/Main.cpp
            src/Framework/Engine/Engine.cpp
            src/Framework/Window/Window.cpp
            src/Framework/Cvar/Cvar.cpp
            src/Framework/Command/Command.cpp
            src/Framework/Events/EventManager.cpp
            src/Framework/Filesystem/FileSystem.cpp
            src/Framework/Platform/Linux/Linux.cpp
            src/Framework/RenderFrontend/RenderFrontend.cpp
            src/Framework/RenderBackend/RenderBackend.cpp
            src/Framework/RenderBackend/RenderPassManager.cpp
            src/Framework/RenderBackend/RenderGraphImage.cpp
            src/Framework/RenderBackend/RenderGraph.cpp
            src/Framework/RenderBackend/Geometry.cpp
            src/Framework/VulkanBackend/VulkanBackend.cpp
            src/Framework/VulkanBackend/Vulkan/VulkanInstance.cpp
            src/Framework/VulkanBackend/Vulkan/Buffer.cpp
            src/Framework/VulkanBackend/Vulkan/StagingBuffer.cpp
            src/Framework/VulkanBackend/Vulkan/VulkanDevice.cpp
            src/Framework/VulkanBackend/Vulkan/SwapChain.cpp
	        src/Framework/VulkanBackend/Vulkan/Image.cpp
	        src/Framework/VulkanBackend/Vulkan/VulkanRenderPass.cpp
	        src/Framework/VulkanBackend/Vulkan/VulkanPipeline.cpp
	        src/Framework/VulkanBackend/Vulkan/CommandBuffer.cpp
            src/Framework/GameWorld/GameWorld.cpp
            src/Framework/Entities/Components/Renderable.cpp
            src/Framework/Entities/Entity.cpp
            src/Framework/VulkanBackend/VulkanRenderPassManager.cpp)

set(CMAKE_BUILD_TYPE Debug)

add_executable(engine ${SOURCES})
set(TARGET_NAME engine)

find_package(SDL2 REQUIRED)
include_directories(engine ${SDL2_INCLUDE_DIRS})

find_package(Vulkan REQUIRED)

target_link_libraries(engine ${SDL2_LIBRARIES})
target_link_libraries(engine Vulkan::Vulkan)

target_compile_definitions(engine PUBLIC RENDERER_VULKAN)
target_compile_options(engine PUBLIC -Wno-nullability-completeness)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set_target_properties(engine PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}
)
