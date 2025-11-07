cmake_minimum_required(VERSION 3.20)
project(cpt64 VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_TESTING "Build tests" ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(${CMAKE_SOURCE_DIR}/project.conf)
configure_file(
    ${CMAKE_SOURCE_DIR}/include/config.h.in
    ${CMAKE_BINARY_DIR}/include/config.h
)

include(FetchContent)

# Directory per dipendenze esterne
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/external")

message(STATUS "Fetching raylib...")
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

message(STATUS "Fetching raygui...")
FetchContent_Declare(
    raygui
    GIT_REPOSITORY https://github.com/raysan5/raygui.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raygui)

# raygui è header-only
add_library(raygui INTERFACE)
target_include_directories(raygui INTERFACE ${raygui_SOURCE_DIR}/src)

include_directories(
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

add_subdirectory(src/core)
add_subdirectory(src/cpu)
add_subdirectory(src/memory)
add_subdirectory(src/video)
add_subdirectory(src/cia)
add_subdirectory(src/log)

add_subdirectory(src/app)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

if(UNIX AND NOT APPLE)
    message(STATUS "Configuring for Linux")
elseif(APPLE)
    message(WARNING "macOS build not yet supported")
elseif(WIN32)
    message(WARNING "Windows build not yet supported")
endif()

message(STATUS "Project: ${PROJECT_NAME} Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
