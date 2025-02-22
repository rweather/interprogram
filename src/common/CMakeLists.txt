list(APPEND COMMON_SOURCES
    ip_ast.c
    ip_ast.h
    ip_errors.c
    ip_exec.c
    ip_exec.h
    ip_labels.c
    ip_labels.h
    ip_listing.c
    ip_parser.c
    ip_parser.h
    ip_program.c
    ip_program.h
    ip_string.c
    ip_string.h
    ip_token.c
    ip_token.h
    ip_types.h
    ip_value.c
    ip_value.h
    ip_vars.c
    ip_vars.h
)
add_library(interprogram-common STATIC ${COMMON_SOURCES})
