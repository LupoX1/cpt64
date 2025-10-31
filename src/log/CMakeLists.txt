add_library(log STATIC
    log.c
    log_stdout.c
)

# Backend raylib solo se compiliamo GUI
if(BUILD_GUI)
    target_sources(log PRIVATE log_raylib.c)
    target_link_libraries(log PUBLIC raylib)
endif()

target_include_directories(log PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_include_directories(log PRIVATE
    ${CMAKE_SOURCE_DIR}/src/log
)