if(NCURSES_FOUND)
    list(APPEND CONSOLE_SOURCES
        ip_console.h
        ip_curses.c
    )
else()
    list(APPEND CONSOLE_SOURCES
        ip_console.h
        ip_no_console.c
    )
endif()
add_library(interprogram-console STATIC ${CONSOLE_SOURCES})
