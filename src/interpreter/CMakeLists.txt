add_executable(interprogram
    main.c
)
target_link_libraries(interprogram PUBLIC interprogram-common)
install(TARGETS interprogram DESTINATION bin)
