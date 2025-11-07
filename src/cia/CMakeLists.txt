add_library(cia STATIC
    cia.c
)

target_include_directories(cia PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cia PUBLIC
    log
)