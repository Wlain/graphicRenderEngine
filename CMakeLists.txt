cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(simpleRenderEngine)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 设置指定的C++版本
# modify flat to -std=c++17 rather than -std=gnu++17
set(CMAKE_CXX_EXTENSIONS OFF)

# glfw
add_subdirectory(3rdparty/glfw)
# glew
add_subdirectory(3rdparty/glew/build/cmake)
# glm
add_subdirectory(3rdparty/glm)
# spdlog
add_subdirectory(3rdparty/spdlog)
# json11
add_subdirectory(3rdparty/json11)
# box2d
add_subdirectory(3rdparty/box2d)
# lua
add_subdirectory(3rdparty/lua)
# sol2
add_subdirectory(3rdparty/sol2)
# render engine
add_subdirectory(ceres)
# Add the cmake folder so the FindSphinx module is found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
add_subdirectory(docs)

if (APPLE)
    INCLUDE_DIRECTORIES(/System/Library/Frameworks)
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
    MARK_AS_ADVANCED(OpenGL_LIBRARY)
    SET(EXTRA_LIBS ${OpenGL_LIBRARY})
endif ()

include_directories(
        ${PROJECT_SOURCE_DIR}/test
        ${PROJECT_SOURCE_DIR}/3rdparty
        ${PROJECT_SOURCE_DIR}/3rdparty/glew/include
        ${PROJECT_SOURCE_DIR}/3rdparty/imgui
        ${PROJECT_SOURCE_DIR}/3rdparty/imgui/backends)

# imgui
file(GLOB IMAGE_UI_SRC
        3rdparty/imgui/backends/imgui_impl_glfw.*
        3rdparty/imgui/backends/imgui_impl_opengl3.*
        3rdparty/imgui/*.cpp)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${IMAGE_UI_SRC} ceres/*.cpp test/*.cpp)

# resources copy
file(COPY ./test/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ceres/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

enable_testing()
add_executable(simpleRenderEngine ${SOURCES})
# 链接库
target_link_libraries(simpleRenderEngine ceres)
