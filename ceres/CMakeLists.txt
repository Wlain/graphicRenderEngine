cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

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

if (APPLE)
    INCLUDE_DIRECTORIES(/System/Library/Frameworks)
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
    MARK_AS_ADVANCED(OpenGL_LIBRARY)
    SET(EXTRA_LIBS ${OpenGL_LIBRARY})
endif ()

# 操作系统检测
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Configuring on Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Configuring on macOS")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Configuring on Windows")
elseif (CMAKE_SYSTEM_NAME STREQUAL "AIX")
    message(STATUS "Configuring on IBM AIX")
else ()
    message(STATUS "Configuring on/for ${CMAKE_SYSTEM_NAME}")
endif ()

message("CMAKE_CURRENT_SOURCE_DIR is" + ${CMAKE_CURRENT_SOURCE_DIR})
message("CMAKE_CURRENT_BINARY_DIR is" + ${CMAKE_CURRENT_BINARY_DIR})
message("CMAKE_CURRENT_LIST_DIR is" + ${CMAKE_CURRENT_LIST_DIR})

file(GLOB IMGUI_SRC
        ${CMAKE_CURRENT_LIST_DIR}/3rdparty/imgui/backends/imgui_impl_glfw.*
        ${CMAKE_CURRENT_LIST_DIR}/3rdparty/imgui/backends/imgui_impl_opengl3.*
        ${CMAKE_CURRENT_LIST_DIR}/3rdparty/imgui/*.cpp)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
        ${IMGUI_SRC}
        ${CMAKE_CURRENT_LIST_DIR}/core/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/utils/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ceres/utils/*.h)

# 将指定的源码编译到库中
add_library(ceres SHARED)

# 头文件目录
target_include_directories(ceres  PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/backend/handle
        ${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl
        ${CMAKE_CURRENT_LIST_DIR}/3rdparty
        ${CMAKE_CURRENT_LIST_DIR}/3rdparty/glew/include
        ${CMAKE_CURRENT_LIST_DIR}/3rdparty/imgui
        ${CMAKE_CURRENT_LIST_DIR}/3rdparty/imgui/backends)

# 源文件目录
target_sources(ceres PUBLIC ${SOURCE_FILES})

include(CheckCXXCompilerFlag)
set(ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_REQUIRED_FLAGS ${ASAN_FLAGS})
check_cxx_compiler_flag(${ASAN_FLAGS} asan_works)
unset(CMAKE_REQUIRED_FLAGS)
target_compile_options(ceres  PUBLIC  ${CXX_BASIC_FLAGS} ${_asan_flags})
# 获取当前操作系统
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(ceres PUBLIC "IS_LINUX")
endif ()
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_compile_definitions(ceres PUBLIC "IS_MACOS")
endif ()
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(ceres PUBLIC "IS_WINDOWS")
endif ()

target_link_libraries(ceres glfw glew glm spdlog json11 ${_asan_flags})
