list(APPEND MATH_SOURCES
    ip_bitwise.c
    ip_math_lib.c
    ip_math_lib.h
    ip_other.c
    ip_power.c
    ip_round.c
    ip_trig.c
)
add_library(interprogram-math STATIC ${MATH_SOURCES})
