cmake_minimum_required(VERSION 3.25)
project(moonshine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "D:/boost_1_85_0")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "D:/boost_1_85_0/stage/lib")
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_COMPILER "vc143")

add_subdirectory(deps/src/glfw)
add_subdirectory(deps/src/spdlog)
add_subdirectory(deps/src/boost EXCLUDE_FROM_ALL)
add_subdirectory(deps/src/miniupnp/miniupnpc)
add_subdirectory(deps/src/tinygltf)

add_executable(${PROJECT_NAME} main.cpp
        editor/Settings.h
        utils/Constants.h
        MoonshineApp.cpp
        MoonshineApp.h
        utils/VkValidationLayerUtils.h
        utils/FileUtils.h
        utils/VkUtils.h
        utils/BufferUtils.h
        editor/Camera.cpp
        editor/Camera.h
        editor/Transform.h
        editor/InputHandler.cpp
        editor/InputHandler.h
        editor/Time.h
        editor/Time.cpp
        utils/InputUtils.h
        utils/CommandBufferUtils.h
        editor/ModelLoader.h
        editor/SceneObject.cpp
        editor/SceneObject.h
        editor/ui/SceneGraph.cpp
        editor/ui/SceneGraph.h
        editor/ui/UIWindow.cpp
        editor/ui/UIWindow.h
        #net/LobbyConnector.cpp
        #net/LobbyConnector.h
        #net/TcpConnection.cpp
        #net/TcpConnection.h
        #net/Client.cpp
        #net/Client.h
        #net/Server.cpp
        #net/Server.h
        #editor/RequestResolver.cpp
        #editor/RequestResolver.h
        editor/ui/LobbyManager.cpp
        editor/ui/LobbyManager.h
        utils/SafeQueue.h
        editor/Scene.cpp
        editor/Scene.h
        editor/ui/WorkspaceManager.cpp
        editor/ui/WorkspaceManager.h
        # MoonshineTodo: Add Im Gui File Dialog
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
        graphics/gl/GLApp.cpp
        graphics/gl/GLApp.h
        graphics/gl/GLGuiRenderer.h
        graphics/gl/GLGuiRenderer.cpp
        graphics/gl/GLShader.h
        graphics/gl/GLProgram.cpp
        graphics/gl/GLProgram.h
        graphics/gl/GLBuffer.cpp
        graphics/gl/GLBuffer.h
        graphics/gl/GLShader.cpp
        graphics/gl/GLUtils.h
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

target_link_libraries(${PROJECT_NAME} 
        glad 
        glfw
        Boost::asio
        Boost::json
        Boost::bind
        Boost::thread
        spdlog::spdlog 
        miniupnpc::miniupnpc
        tinygltf)

target_include_directories(glad PRIVATE "${CMAKE_SOURCE_DIR}/deps/glad/include")
include_directories(${CMAKE_SOURCE_DIR}/deps/src/glm
        ${CMAKE_SOURCE_DIR}/deps/src/stb
        deps/src/imgui
        ${Boost_INCLUDE_DIR})


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


