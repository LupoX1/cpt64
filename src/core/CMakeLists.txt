add_library(core STATIC
    app.c
)

target_include_directories(core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
