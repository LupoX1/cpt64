add_library(log STATIC
    log.c
)

target_link_libraries(log PUBLIC raylib)

target_include_directories(log PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)