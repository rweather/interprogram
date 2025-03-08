list(APPEND STRING_SOURCES
    ip_string_lib.c
    ip_string_lib.h
)
add_library(interprogram-string STATIC ${STRING_SOURCES})
