add_library(cpu STATIC
    cpu.c
    addressing.c
    instructions.c
    opcodes.c
)

target_include_directories(cpu PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(cpu PUBLIC
    log
)