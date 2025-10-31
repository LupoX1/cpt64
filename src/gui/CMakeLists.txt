add_executable(cpt64-gui
    main.c
)

target_link_libraries(cpt64-gui PRIVATE
    core
    log
    video
    raylib
    raygui
    m
)

target_compile_definitions(cpt64-gui PRIVATE
    USE_RAYLIB
    USE_RAYGUI
    $<$<CONFIG:Debug>:DEBUG>
)