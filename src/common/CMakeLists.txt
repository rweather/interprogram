list(APPEND COMMON_SOURCES
    ip_ast.c
    ip_ast.h
    ip_errors.c
    ip_labels.c
    ip_labels.h
    ip_parser.c
    ip_parser.h
    ip_program.c
    ip_program.h
    ip_token.c
    ip_token.h
    ip_types.h
    ip_vars.c
    ip_vars.h
)
add_library(interprogram-common STATIC ${COMMON_SOURCES})
