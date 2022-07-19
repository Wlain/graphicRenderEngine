cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

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

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/ceres/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ceres/utils/*.h)

# 将指定的源码编译到库中
add_library(ceres SHARED)

# 头文件目录
target_include_directories(ceres  PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
        )
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

target_link_libraries(ceres glfw glew glm spdlog json11 box2d liblua sol2 ${_asan_flags})
