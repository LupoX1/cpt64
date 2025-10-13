cmake_minimum_required(VERSION 3.20)

# Leggi la configurazione dal file project.conf
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

# Cartella per le dipendenze esterne (non viene eliminata con clean)
set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external" CACHE PATH "External dependencies directory")
set(FETCHCONTENT_FULLY_DISCONNECTED OFF CACHE BOOL "Allow FetchContent to download")

# Determina il sistema operativo
if(WIN32)
    set(PLATFORM "windows")
elseif(APPLE)
    set(PLATFORM "macos")
else()
    set(PLATFORM "linux")
endif()

message(STATUS "Platform: ${PLATFORM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Use SDL: ${USE_SDL}")
message(STATUS "External deps: ${FETCHCONTENT_BASE_DIR}")

# ============================================================================
# Opzioni di compilazione
# ============================================================================

option(USE_SDL "Usa SDL3" ON)

# Flags di compilazione per Debug e Release
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

if(MSVC)
    set(CMAKE_C_FLAGS_DEBUG "/Zi /Od /W4")
    set(CMAKE_C_FLAGS_RELEASE "/O2")
endif()

# ============================================================================
# Gestione delle dipendenze (SDL3)
# ============================================================================

if(USE_SDL)
    find_package(SDL3 QUIET)
    
    if(NOT SDL3_FOUND)
        message(STATUS "SDL3 non trovata, scarico automaticamente...")
        include(FetchContent)
        FetchContent_Declare(SDL3
            URL https://github.com/libsdl-org/SDL/releases/download/release-3.2.24/SDL3-3.2.24.tar.gz
        )
        FetchContent_MakeAvailable(SDL3)
        
        # Imposta SDL3_LIBRARIES e SDL3_INCLUDE_DIRS manualmente se necessario
        if(NOT SDL3_LIBRARIES)
            set(SDL3_LIBRARIES SDL3::SDL3)
        endif()
    endif()
endif()

# ============================================================================
# Directory di include
# ============================================================================

set(COMMON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(${COMMON_INCLUDE_DIR})

# ============================================================================
# Struttura dei moduli
# ============================================================================

# File sorgenti da tutti i moduli
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# ============================================================================
# Generazione del file config con versione
# ============================================================================

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# ============================================================================
# Creazione dell'eseguibile
# ============================================================================

add_executable(${PROJECT_NAME} ${SOURCES})

# Aggiungi la flag SDL al compilatore se abilitato
if(USE_SDL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_SDL=1)
    target_link_libraries(${PROJECT_NAME} SDL3::SDL3)
endif()

# Include per il progetto principale
target_include_directories(${PROJECT_NAME} PRIVATE ${COMMON_INCLUDE_DIR})

# ============================================================================
# Installazione
# ============================================================================

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY ${COMMON_INCLUDE_DIR}/ DESTINATION include)

# ============================================================================
# Output
# ============================================================================

message(STATUS "Target: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Description: ${PROJECT_DESCRIPTION}")
message(STATUS "Output directory: ${CMAKE_BINARY_DIR}")

# ============================================================================
# Target custom per pulizia selettiva
# ============================================================================

add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Pulizia completa (build eliminato, external conservato)"
)

add_custom_target(clean-external
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${FETCHCONTENT_BASE_DIR}"
    COMMENT "Eliminazione dipendenze esterne"
)
