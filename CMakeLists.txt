cmake_minimum_required(VERSION 3.25)
project(moonshine)

set(CMAKE_CXX_STANDARD 17)

SET(EASY_PROFILER_NO_GUI OFF CACHE BOOL "")
SET(EASY_PROFILER_NO_SAMPLES ON CACHE BOOL "")

set(BOOST_JSON_STATIC_LINK, ON)

# Ignore New Boost version may have incorrect or missing dependencies and imported
set(Boost_NO_WARN_NEW_VERSIONS 1)

set(IMGUI_DIR external/imgui/)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)

set(IMGUI_FILE_DIR external/ImGuiFileDialog/)
include_directories(${IMGUI_FILE_DIR} ${IMGUI_FILE_DIR})

set(spdlog_DIR "external/spdlog/cmake-build-release")

add_executable(${PROJECT_NAME} main.cpp editor/Settings.h utils/Constants.h MoonshineApp.cpp MoonshineApp.h utils/VkValidationLayerUtils.h utils/FileUtils.h utils/VkUtils.h graphics/Window.cpp graphics/Window.h graphics/Device.cpp graphics/Device.h graphics/Pipeline.cpp graphics/Pipeline.h graphics/GpuBuffer.h utils/BufferUtils.h editor/Camera.cpp editor/Camera.h editor/Transform.h editor/InputHandler.cpp editor/InputHandler.h editor/Time.h editor/Time.cpp utils/InputUtils.h graphics/TextureImage.cpp graphics/TextureImage.h utils/CommandBufferUtils.h graphics/TextureSampler.cpp graphics/TextureSampler.h graphics/Buffer.cpp graphics/Buffer.h editor/ModelLoader.h editor/SceneObject.cpp editor/SceneObject.h graphics/Renderer.cpp graphics/Renderer.h graphics/SwapChain.cpp graphics/SwapChain.h graphics/SimpleRenderSystem.cpp graphics/SimpleRenderSystem.h graphics/Descriptors.cpp graphics/Descriptors.h graphics/FrameInfo.h "graphics/UniformBuffer.h" ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp
        graphics/Material.cpp
        graphics/Material.h
        graphics/MaterialManager.cpp
        graphics/MaterialManager.h
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
        external/ImGuiFileDialog/ImGuiFileDialog.h
        external/ImGuiFileDialog/ImGuiFileDialog.cpp
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
        graphics/Gui.cpp
        graphics/Gui.h
        graphics/Gui.h
        graphics/Gui.cpp
)

# DOWNLOAD ALL THE SUBMODULES
find_package(Git QUIET)
# if (GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
#    #Update submodules as needed
#    option(GIT_SUBMODULE "Check submodules during build" ON)
#    if (GIT_SUBMODULE)
#        message(STATUS "Submodule update")
#        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
#                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#                RESULT_VARIABLE GIT_SUBMODULE_RESULT)
#        if (NOT GIT_SUBMODULE_RESULT EQUAL "0")
#            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMODULE_RESULT}, please checkout")
#        endif ()
#    endif ()
#endif ()

# Specify the required Boost version
find_package(Boost 1.83.0 REQUIRED json thread random)
find_package(miniupnpc CONFIG REQUIRED)
if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

include_directories(external/stb/)

# Add and link libraries
add_subdirectory(external/glfw)
add_subdirectory(external/glm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/easy_profiler)

set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
add_subdirectory(external/tinygltf)

if (MINGW)
    target_link_libraries(${PROJECT_NAME} ws2_32 wsock32)
endif ()

target_link_libraries(${PROJECT_NAME} easy_profiler)  # STEP 2 ##########

target_include_directories(${PROJECT_NAME}
        PUBLIC external/glfw/include
        PUBLIC external/glm/glm
        PUBLIC ${Boost_INCLUDE_DIR}
)

target_link_directories(${PROJECT_NAME}
        PRIVATE external/glfw/src
)

message(STATUS "Boost Libraries: ${Boost_INCLUDE_DIR}")
message(STATUS "Boost Libraries: ${Boost_LIBRARIES}")


target_link_libraries(${PROJECT_NAME}
        glfw
        glm
        ${Boost_LIBRARIES}
        miniupnpc::miniupnpc
        bcrypt # required for boost random
        spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>
)


# https://github.com/PacktPublishing/Learning-Vulkan/blob/master/Chapter%2003/HandShake/CMakeLists.txt
# AUTO_LOCATE_VULKAN - accepted value ON or OFF
# ON  - Use CMake to auto locate the Vulkan SDK.
# OFF - Vulkan SDK path can be specified manually. This is helpful to test the build on various Vulkan version.
option(AUTO_LOCATE_VULKAN "AUTO_LOCATE_VULKAN" ON)

if (AUTO_LOCATE_VULKAN)
    message(STATUS "Attempting auto locate Vulkan using CMake......")

    # Find Vulkan Path using CMake's Vulkan Module
    # This will return Boolean 'Vulkan_FOUND' indicating the status of find as success(ON) or fail(OFF).
    # Include directory path - 'Vulkan_INCLUDE_DIRS' and 'Vulkan_LIBRARY' with required libraries.
    find_package(Vulkan)

    # Try extracting VulkanSDK path from ${Vulkan_INCLUDE_DIRS}
    if (NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
        set(VULKAN_PATH ${Vulkan_INCLUDE_DIRS})
        STRING(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
    endif ()

    if (NOT Vulkan_FOUND)
        # CMake may fail to locate the libraries but could be able to
        # provide some path in Vulkan SDK include directory variable
        # 'Vulkan_INCLUDE_DIRS', try to extract path from this.
        message(STATUS "Failed to locate Vulkan SDK, retrying again...")
        if (EXISTS "${VULKAN_PATH}")
            message(STATUS "Successfully located the Vulkan SDK: ${VULKAN_PATH}")
        else ()
            message("Error: Unable to locate Vulkan SDK. Please turn off auto locate option by specifying 'AUTO_LOCATE_VULKAN' as 'OFF'")
            message("and specify manually path using 'VULKAN_SDK' and 'VULKAN_VERSION' variables in the CMakeLists.txt.")
            return()
        endif ()
    endif ()
else ()
    message(STATUS "Attempting to locate Vulkan SDK using manual path......")
    set(VULKAN_SDK "C:/VulkanSDK")
    set(VULKAN_VERSION "1.3.239.0")
    set(VULKAN_PATH "${VULKAN_SDK}/${VULKAN_VERSION}")
    message(STATUS "Using manual specified path: ${VULKAN_PATH}")

    # Check if manual set path exists
    if (NOT EXISTS "${VULKAN_PATH}")
        message("Error: Unable to locate this Vulkan SDK path VULKAN_PATH: ${VULKAN_PATH}, please specify correct path.
		For more information on correct installation process, please refer to subsection 'Getting started with Lunar-G SDK'
		and 'Setting up first project with CMake' in Chapter 3, 'Shaking hands with the device' in this book 'Learning Vulkan', ISBN - 9781786469809.")
        return()
    endif ()
endif ()

# Add any required preprocessor definitions here
add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)

# vulkan-1 library for build Vulkan application.
set(VULKAN_LIB_LIST "vulkan-1")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Include Vulkan header files from Vulkan SDK
    TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC AFTER ${VULKAN_PATH}/Include)

    # Link directory for vulkan-1
    TARGET_LINK_DIRECTORIES(${PROJECT_NAME} PRIVATE ${VULKAN_PATH}/Bin;${VULKAN_PATH}/Lib;)
endif ()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${VULKAN_LIB_LIST})

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


