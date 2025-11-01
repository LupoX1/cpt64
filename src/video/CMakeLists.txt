add_library(video STATIC
    video.c
    vic.c
)

target_include_directories(video PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(video PUBLIC
    log
)