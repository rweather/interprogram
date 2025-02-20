
enable_testing()

add_executable(token-test token-test.c)
target_link_libraries(token-test PUBLIC interprogram-common m)
add_test(NAME token-test COMMAND token-test)
