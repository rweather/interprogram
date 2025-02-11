list(APPEND COMMON_SOURCES
    ip_token.c
    ip_token.h
)
add_library(interprogram STATIC ${COMMON_SOURCES})
