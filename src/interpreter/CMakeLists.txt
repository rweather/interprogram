add_executable(interprogram
    main.c
)
target_link_libraries(
    interprogram
    PUBLIC interprogram-math interprogram-common m
)
install(TARGETS interprogram DESTINATION bin)
