add_executable(interprogram
    main.c
)
target_include_directories(
    interprogram
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/../common
        ${CMAKE_CURRENT_LIST_DIR}/../math
        ${CMAKE_CURRENT_LIST_DIR}/../string
)
target_link_libraries(
    interprogram
    PUBLIC interprogram-math interprogram-string interprogram-common m
)
install(TARGETS interprogram DESTINATION bin)
