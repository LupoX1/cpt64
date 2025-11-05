add_executable(test_core test_core.c)
target_link_libraries(test_core PRIVATE core log)
add_test(NAME test_core COMMAND test_core)

add_executable(test_log test_log.c)
target_link_libraries(test_log PRIVATE log)
add_test(NAME test_log COMMAND test_log)

add_executable(test_video test_video.c)
target_link_libraries(test_video PRIVATE video log)
add_test(NAME test_video COMMAND test_video)

# Test Runner con bus alternativo
# Dobbiamo compilare anche system.c perché contiene c64_create/destroy
add_executable(test_runner 
    test_runner.c
    bus_test.c
    ${CMAKE_SOURCE_DIR}/src/core/system.c
)

target_include_directories(test_runner PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(test_runner PRIVATE
    cpu
    memory
    video
    log
)

# Copia le risorse dei test nella cartella build
add_custom_command(TARGET test_runner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/tests/resources
    ${CMAKE_BINARY_DIR}/tests/resources
    COMMENT "Copying test resources to build directory"
)

add_test(NAME test_functional 
         COMMAND test_runner
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
         
add_test(NAME test_interrupt 
         COMMAND test_runner --interrupt
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR})