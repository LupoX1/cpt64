add_executable(cpt64
    main.c
)

target_link_libraries(cpt64 PRIVATE
    core
    log
    video
    raylib
    raygui
    m
)

target_compile_definitions(cpt64 PRIVATE
    USE_RAYLIB
    USE_RAYGUI
    $<$<CONFIG:Debug>:DEBUG>
)