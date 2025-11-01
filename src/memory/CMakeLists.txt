add_library(memory STATIC
    memory.c
)

target_include_directories(memory PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(memory PUBLIC
    log
)