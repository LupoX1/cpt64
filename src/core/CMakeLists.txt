add_library(core STATIC
    app.c
    system.c
    bus.c
)

target_include_directories(core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(core PUBLIC
    log
    cpu
    memory
    video
)