cmake_minimum_required(VERSION 3.22)

project(SnakeRaylibAndroid C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT ANDROID)
    message(FATAL_ERROR "This project must be configured with the Android NDK toolchain.")
endif()

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(PLATFORM Android CACHE STRING "" FORCE)
set(OPENGL_VERSION "ES 2.0" CACHE STRING "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(raylib)

add_library(main SHARED main.c)

set_target_properties(main PROPERTIES
    OUTPUT_NAME "main"
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)

target_include_directories(main PRIVATE
    ${raylib_SOURCE_DIR}/src
)

target_compile_definitions(main PRIVATE
    PLATFORM_ANDROID
)

target_link_libraries(main PRIVATE
    raylib
    android
    log
    EGL
    GLESv2
)

set_target_properties(main PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/apk-lib/arm64-v8a"
)
