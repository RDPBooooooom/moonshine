cmake_minimum_required(VERSION 3.25)
project(moonshine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

add_subdirectory(deps/src/glfw)

add_executable(${PROJECT_NAME} main.cpp editor/Settings.h utils/Constants.h MoonshineApp.cpp MoonshineApp.h utils/VkValidationLayerUtils.h utils/FileUtils.h utils/VkUtils.h graphics/vk/Window.cpp graphics/vk/Window.h graphics/vk/Device.cpp graphics/vk/Device.h graphics/vk/Pipeline.cpp graphics/vk/Pipeline.h graphics/vk/GpuBuffer.h utils/BufferUtils.h editor/Camera.cpp editor/Camera.h editor/Transform.h editor/InputHandler.cpp editor/InputHandler.h editor/Time.h editor/Time.cpp utils/InputUtils.h graphics/vk/TextureImage.cpp graphics/vk/TextureImage.h utils/CommandBufferUtils.h graphics/vk/TextureSampler.cpp graphics/vk/TextureSampler.h graphics/vk/Buffer.cpp graphics/vk/Buffer.h editor/ModelLoader.h editor/SceneObject.cpp editor/SceneObject.h graphics/vk/Renderer.cpp graphics/vk/Renderer.h graphics/vk/SwapChain.cpp graphics/vk/SwapChain.h graphics/vk/SimpleRenderSystem.cpp graphics/vk/SimpleRenderSystem.h graphics/vk/Descriptors.cpp graphics/vk/Descriptors.h graphics/vk/FrameInfo.h "graphics/vk/UniformBuffer.h"
        graphics/vk/Material.cpp
        graphics/vk/Material.h
        graphics/vk/MaterialManager.cpp
        graphics/vk/MaterialManager.h
        editor/ui/SceneGraph.cpp
        editor/ui/SceneGraph.h
        editor/ui/UIWindow.cpp
        editor/ui/UIWindow.h
        net/LobbyConnector.cpp
        net/LobbyConnector.h
        net/TcpConnection.cpp
        net/TcpConnection.h
        editor/ui/LobbyManager.cpp
        editor/ui/LobbyManager.h
        utils/SafeQueue.h
        net/Client.cpp
        net/Client.h
        net/Server.cpp
        net/Server.h
        editor/Scene.cpp
        editor/Scene.h
        editor/RequestResolver.cpp
        editor/RequestResolver.h
        editor/ui/WorkspaceManager.cpp
        editor/ui/WorkspaceManager.h
        # Add Im Gui File Dialog
        #external/ImGuiFileDialog/ImGuiFileDialog.h
        #external/ImGuiFileDialog/ImGuiFileDialog.cpp
        editor/GltfLoader.cpp
        editor/GltfLoader.h
        editor/Mesh.h
        editor/ModelLoader.cpp
        editor/Node.cpp
        editor/Node.h
        editor/EngineSystems.h
        editor/logging/Logger.h
        editor/logging/UILogger.h
        editor/logging/LoggerType.h
        editor/logging/LogMessage.h
        editor/logging/LoggerType.cpp
        editor/logging/Logger.cpp
        editor/ui/StatisticsManager.cpp
        editor/ui/StatisticsManager.h
        editor/ui/net/InputFloat3.cpp
        editor/ui/net/InputFloat3.h
        editor/ui/net/UIManager.cpp
        editor/ui/net/UIManager.h
        graphics/vk/Gui.cpp
        graphics/vk/Gui.h
        graphics/vk/Gui.h
        graphics/vk/Gui.cpp
        graphics/gl/GLApp.cpp
        graphics/gl/GLApp.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME_DEBUG ${PROJECT_NAME}_Debug)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME_RELEASE ${PROJECT_NAME}_Release)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME_RELWITHDEBINFO ${PROJECT_NAME}_ReleaseDebInfo)

# Handle glad
set(GLAD_SOURCES deps/glad/src/gl.c deps/glad/src/vulkan.c)
if (WIN32)
    set(GLAD_SOURCES ${GLAD_SOURCES} deps/glad/src/wgl.c)
endif ()
add_library(glad ${GLAD_SOURCES})

target_link_libraries(${PROJECT_NAME} glad glfw)

target_include_directories(glad PRIVATE "${CMAKE_SOURCE_DIR}/deps/glad/include"
        deps/src/glm
        deps/src/stb
        deps/src/imgui
)


# Compiling Shaders to spir-v
if (WIN32)
    message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
    message(STATUS "Compiling shaders")
    add_custom_target(CompileShaders ALL
            COMMAND ${CMAKE_COMMAND} -E echo "Running compile.bat"
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR} cmd.exe /C compile.bat resources/shaders/ resources/shaders/
            COMMENT "Compiling shaders with compile.bat"
    )
    add_dependencies(${PROJECT_NAME} CompileShaders)
endif ()

add_custom_target(
        CopyResources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources
        COMMENT "Copying resources to build directory"
)

add_dependencies(${PROJECT_NAME} CopyResources)
add_dependencies(CopyResources CompileShaders)


