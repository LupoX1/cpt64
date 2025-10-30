add_executable(cpt64-cli
    main.c
)

target_link_libraries(cpt64-cli PRIVATE
    core
    log
    video
)
