add_library(log STATIC
    log.c
    log_raylib.c
)

target_include_directories(log PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/log
)