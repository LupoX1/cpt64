add_executable(cpt64-cli
    main.c
)

target_link_libraries(cpt64-cli PRIVATE
    core
    log
    video
)

target_compile_definitions(cpt64-cli PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)