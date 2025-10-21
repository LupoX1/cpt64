cmake_minimum_required(VERSION 3.20)

# Carica configurazione progetto
include(${CMAKE_CURRENT_SOURCE_DIR}/project.conf)

project(
    ${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    DESCRIPTION "${PROJECT_DESCRIPTION}"
    LANGUAGES C
)

# ============================================================================
# Configurazioni generali
# ============================================================================

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Directory per dipendenze esterne
set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external" 
    CACHE PATH "External dependencies directory")

# Determina piattaforma
if(WIN32)
    set(PLATFORM "windows")
elseif(APPLE)
    set(PLATFORM "macos")
else()
    set(PLATFORM "linux")
endif()

message(STATUS "=== C64 Emulator Build Configuration ===")
message(STATUS "Platform: ${PLATFORM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")

# ============================================================================
# Opzioni
# ============================================================================

option(USE_SDL "Enable SDL3 support" ON)
option(BUILD_TESTS "Build test suite" ON)

message(STATUS "USE_SDL: ${USE_SDL}")
message(STATUS "BUILD_TESTS: ${BUILD_TESTS}")

# ============================================================================
# Flags di compilazione
# ============================================================================

set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

if(MSVC)
    set(CMAKE_C_FLAGS_DEBUG "/Zi /Od /W4")
    set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG")
endif()

# ============================================================================
# Dipendenze esterne (SDL3)
# ============================================================================

if(USE_SDL)
    find_package(SDL3 QUIET)
    
    if(NOT SDL3_FOUND)
        message(STATUS "SDL3 not found, downloading...")
        include(FetchContent)
        FetchContent_Declare(SDL3
            URL https://github.com/libsdl-org/SDL/releases/download/release-3.2.24/SDL3-3.2.24.tar.gz
            DOWNLOAD_EXTRACT_TIMESTAMP ON
        )
        FetchContent_MakeAvailable(SDL3)
        message(STATUS "SDL3 downloaded to ${FETCHCONTENT_BASE_DIR}")
    else()
        message(STATUS "Using system SDL3")
    endif()
endif()

# ============================================================================
# Libreria Core C64
# ============================================================================

add_library(c64_core STATIC
    # Core
    src/core/bus.c
    src/core/system.c
    
    # Memory
    src/memory/memory.c
    
    # CPU
    src/cpu/cpu.c
    src/cpu/addressing.c
    src/cpu/opcodes.c
    src/cpu/instructions.c
)

target_include_directories(c64_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Genera config.h
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

# ============================================================================
# Eseguibile principale
# ============================================================================

add_executable(${PROJECT_NAME} src/main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE c64_core)

if(USE_SDL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_SDL=1)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
    message(STATUS "Main executable built with SDL3 support")
else()
    message(STATUS "Main executable built in console mode")
endif()

# ============================================================================
# Test Suite
# ============================================================================

if(BUILD_TESTS)
    add_executable(c64_tests tests/test_runner.c)
    
    target_link_libraries(c64_tests PRIVATE c64_core)
    
    # Copia risorse test
    add_custom_command(TARGET c64_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/resources
            $<TARGET_FILE_DIR:c64_tests>/tests/resources
        COMMENT "Copying test resources to build directory"
    )
    
    message(STATUS "Test suite enabled")
endif()

# ============================================================================
# Copia ROM
# ============================================================================

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/roms 
     DESTINATION ${CMAKE_BINARY_DIR})
message(STATUS "ROMs copied to build directory")

# ============================================================================
# Installazione
# ============================================================================

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY include/ DESTINATION include 
        FILES_MATCHING PATTERN "*.h")

if(BUILD_TESTS)
    install(TARGETS c64_tests DESTINATION bin)
endif()

# ============================================================================
# Target custom
# ============================================================================

# Clean completo (build + external)
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${FETCHCONTENT_BASE_DIR}"
    COMMENT "Cleaning build and external dependencies"
)

# ============================================================================
# Summary
# ============================================================================

message(STATUS "")
message(STATUS "=== Build Summary ===")
message(STATUS "Target: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Output directory: ${CMAKE_BINARY_DIR}")
message(STATUS "")
message(STATUS "Build with:")
message(STATUS "  cmake --build ${CMAKE_BINARY_DIR}")
message(STATUS "")
message(STATUS "Run tests:")
message(STATUS "  ${CMAKE_BINARY_DIR}/c64_tests")
message(STATUS "")