cmake_minimum_required(VERSION 3.22)

project(SnakeRaylibAndroid C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT ANDROID)
    message(FATAL_ERROR "This project must be configured with the Android NDK toolchain.")
endif()

include(FetchContent)

set(RAYLIB_VERSION 6.0)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(PLATFORM Android CACHE STRING "" FORCE)
set(OPENGL_VERSION "ES 2.0" CACHE STRING "" FORCE)

FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
)

FetchContent_MakeAvailable(raylib)

add_library(main SHARED main.c)

set_target_properties(main PROPERTIES
    OUTPUT_NAME "main"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/apk-lib/arm64-v8a"
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)

target_compile_definitions(main PRIVATE PLATFORM_ANDROID)

target_link_options(main PRIVATE
    "-Wl,-u,ANativeActivity_onCreate"
    "-Wl,--wrap=fopen"
    "-Wl,-undefined,dynamic_lookup"
)

target_link_libraries(main PRIVATE
    raylib
    android
    log
    EGL
    GLESv2
)
