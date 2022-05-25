cmake_minimum_required(VERSION 3.5)
project(simpleRenderEngine)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 设置指定的C++版本
# modify flat to -std=c++20 rather than -std=gnu++20
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DCURRENT_PROJECT_DIR="${PROJECT_SOURCE_DIR}/")

# glfw
add_subdirectory(3rdparty/glfw)
# glm
add_subdirectory(3rdparty/glm)
# spdlog
add_subdirectory(3rdparty/spdlog)

if (APPLE)
    INCLUDE_DIRECTORIES(/System/Library/Frameworks)
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
    MARK_AS_ADVANCED(OpenGL_LIBRARY)
    SET(EXTRA_LIBS ${OpenGL_LIBRARY})
endif ()

include_directories(
        ${PROJECT_SOURCE_DIR}/re
        ${PROJECT_SOURCE_DIR}/re/utils
        ${PROJECT_SOURCE_DIR}/test
        ${PROJECT_SOURCE_DIR}/3rdparty
        ${PROJECT_SOURCE_DIR}/3rdparty/imgui
        ${PROJECT_SOURCE_DIR}/3rdparty/imgui/backends)

# imgui
file(GLOB IMAGE_UI_SRC
        3rdparty/imgui/backends/imgui_impl_glfw.*
        3rdparty/imgui/backends/imgui_impl_opengl3.*
        3rdparty/imgui/*.cpp)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${IMAGE_UI_SRC} re/*.cpp test/*.cpp)

add_executable(simpleRenderEngine ${SOURCES})

# 链接库
target_link_libraries(simpleRenderEngine re)

include_directories(re)
add_subdirectory(re)
