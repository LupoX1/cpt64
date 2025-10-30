add_executable(test_core test_core.c)
target_link_libraries(test_core PRIVATE core log)
add_test(NAME test_core COMMAND test_core)

add_executable(test_log test_log.c)
target_link_libraries(test_log PRIVATE log)
add_test(NAME test_log COMMAND test_log)

add_executable(test_video test_video.c)
target_link_libraries(test_video PRIVATE video log)
add_test(NAME test_video COMMAND test_video)
