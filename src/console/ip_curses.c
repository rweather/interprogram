/*
 * Copyright (C) 2025 Rhys Weatherley
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "ip_console.h"
#include <curses.h>
#include <term.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

/** Non-zero once screen initialisation is done */
static int init_screen_done = 0;

/** Current text attributes */
static int text_attributes =
    A_NORMAL | COLOR_PAIR(COLOR_BLACK * 8 + COLOR_WHITE);

/**
 * @brief Ends screen operations when the program terminates.
 */
static void end_screen(void)
{
    if (init_screen_done) {
        endwin();
    }
}

/**
 * @brief Outputs a string to the console.
 *
 * @param[in] exec The execution context.
 * @param[in] str The string to output.
 */
static void console_output_string(ip_exec_t *exec, const char *str)
{
    (void)exec;
    addstr(str);
    refresh();
}

/**
 * @brief Outputs a character to the console.
 *
 * @param[in] exec The execution context.
 * @param[in] ch The character to output.
 */
static void console_output_char(ip_exec_t *exec, int ch)
{
    (void)exec;
    addch(ch);
    refresh();
}

/**
 * @brief Inputs a line of text from the console.
 *
 * @param[in] exec The execution context.
 * @param[in] buf Points to the line buffer.
 * @param[in] size Size of the line buffer.
 */
static void console_input_line(ip_exec_t *exec, char *buf, size_t size)
{
    (void)exec;
    if (getnstr(buf, size) != OK) {
        buf[0] = '\0';
    }
}

/**
 * @brief Deactivate console mode.
 *
 * @param[in] exec The execution context.
 */
static void console_deactivate(ip_exec_t *exec)
{
    if (init_screen_done) {
        exec->output_string = 0;
        exec->output_char = 0;
        exec->input_line = 0;
        exec->deactivate_console = 0;
        init_screen_done = 0;
        endwin();
    }
}

/**
 * @brief Initialises the screen for curses operations.
 *
 * @param[in,out] exec The execution context.
 */
static void init_screen(ip_exec_t *exec)
{
    int fg, bg;
    if (!init_screen_done) {
        exec->output_string = console_output_string;
        exec->output_char = console_output_char;
        exec->input_line = console_input_line;
        exec->deactivate_console = console_deactivate;
        fflush(exec->output);
        if (exec->input != stdin ||
                exec->output != stdout) {
            fputs("Cannot initialise the screen; aborting.\n", stderr);
            exit(3);
        }
        atexit(end_screen);
        setlocale(LC_ALL, "");
        initscr();
        scrollok(stdscr, TRUE);
        keypad(stdscr, TRUE);
        start_color();
        for (fg = 0; fg < 8; ++fg) {
            for (bg = 0; bg < 8; ++bg) {
                init_pair(bg * 8 + fg, fg, bg);
            }
        }
        attrset(text_attributes);
        bkgd(' ' | text_attributes);
        clear();
        refresh();
        init_screen_done = 1;
    }
}

/**
 * @brief Clears the screen.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_clear_screen
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    (void)args;
    (void)num_args;
    init_screen(exec);
    bkgd(' ' | text_attributes);
    clear();
    refresh();
    return IP_EXEC_OK;
}

/**
 * @brief Clears from the current position to the end of the screen.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_clear_eos
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    (void)args;
    (void)num_args;
    init_screen(exec);
    clrtobot();
    refresh();
    return IP_EXEC_OK;
}

/**
 * @brief Clears from the current position to the end of the current line.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_clear_eol
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    (void)args;
    (void)num_args;
    init_screen(exec);
    clrtoeol();
    refresh();
    return IP_EXEC_OK;
}

/**
 * @brief Moves the cursor to a particular location on-screen.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_move_cursor_to
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)num_args;
    init_screen(exec);
    status = ip_value_to_int(&(args[0]));
    if (status == IP_EXEC_OK) {
        status = ip_value_to_int(&(args[1]));
    }
    if (status == IP_EXEC_OK) {
        int maxx, maxy;
        int x = args[0].ivalue;
        int y = args[1].ivalue;
        getmaxyx(stdscr, maxx, maxy);
        if (x < 0) {
            x = 0;
        } else if (x >= maxx) {
            x = maxx - 1;
        }
        if (y < 0) {
            y = 0;
        } else if (y >= maxy) {
            y = maxy - 1;
        }
        move(y, x);
    }
    return status;
}

/**
 * @brief Gets the X position of the cursor.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_cursor_x
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int x, y;
    (void)args;
    (void)num_args;
    init_screen(exec);
    getyx(stdscr, y, x);
    (void)y;
    ip_value_set_int(&(exec->this_value), x);
    return IP_EXEC_OK;
}

/**
 * @brief Gets the Y position of the cursor.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_cursor_y
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int x, y;
    (void)args;
    (void)num_args;
    init_screen(exec);
    getyx(stdscr, y, x);
    (void)x;
    ip_value_set_int(&(exec->this_value), y);
    return IP_EXEC_OK;
}

/**
 * @brief Gets the width of the screen into "THIS".
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_screen_width
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int maxx, maxy;
    (void)args;
    (void)num_args;
    init_screen(exec);
    getmaxyx(stdscr, maxy, maxx);
    (void)maxy;
    ip_value_set_int(&(exec->this_value), maxx);
    return IP_EXEC_OK;
}

/**
 * @brief Gets the height of the screen into "THIS".
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_screen_height
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int maxx, maxy;
    (void)args;
    (void)num_args;
    init_screen(exec);
    getmaxyx(stdscr, maxy, maxx);
    (void)maxx;
    ip_value_set_int(&(exec->this_value), maxy);
    return IP_EXEC_OK;
}

/**
 * @brief Selects normal text attributes.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_normal_text
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    (void)args;
    (void)num_args;
    init_screen(exec);
    text_attributes &= ~A_BOLD;
    text_attributes |= A_NORMAL;
    attrset(text_attributes);
    return IP_EXEC_OK;
}

/**
 * @brief Selects bold text attributes.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_bold_text
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    (void)args;
    (void)num_args;
    init_screen(exec);
    text_attributes &= ~A_NORMAL;
    text_attributes |= A_BOLD;
    attrset(text_attributes);
    return IP_EXEC_OK;
}

/**
 * @brief Selects the foreground and background text colors to use.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_text_color
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    int fg, bg;
    bg = (PAIR_NUMBER(text_attributes) >> 3) & 0x07;
    status = ip_value_to_int(&(args[0]));
    if (status == IP_EXEC_OK) {
        fg = args[0].ivalue & 0x07;
        if (num_args > 1) {
            status = ip_value_to_int(&(args[1]));
            if (status == IP_EXEC_OK) {
                bg = args[1].ivalue & 0x07;
            }
        }
    }
    if (status == IP_EXEC_OK) {
        init_screen(exec);
        text_attributes &= ~A_COLOR;
        text_attributes |= COLOR_PAIR(bg * 8 + fg);
        attrset(text_attributes);
    }
    return status;
}

/**
 * @brief Beeps the speaker.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_beep
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    (void)args;
    (void)num_args;
    init_screen(exec);
    beep();
    return IP_EXEC_OK;
}

/**
 * @brief Flashes the screen.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_flash
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    (void)args;
    (void)num_args;
    init_screen(exec);
    flash();
    return IP_EXEC_OK;
}

/**
 * @brief Inputs a single key.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_input_key
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int time = -1;
    int status;
    int ch;
    ip_string_t *str;

    /* Get the timeout value from the first argument */
    if (num_args > 0) {
        status = ip_value_to_int(&(args[0]));
        if (status != IP_EXEC_OK) {
            return status;
        }
        if (args[0].ivalue >= 0) {
            time = (int)(args[0].ivalue);
        }
    }

    /* Get the next key */
    init_screen(exec);
    timeout(time);
    cbreak();
    noecho();
    ch = getch();
    timeout(-1);
    echo();
    nocbreak();

    /* Determine what to return to the caller based on the key */
    if (ch < 0) {
        /* Timeout occurred, so return the empty string */
        str = ip_string_create_empty();
    } else if (ch >= 0 && ch <= 255) {
        /* Ordinary character */
        char c = (char)ch;
        str = ip_string_create_with_length(&c, 1);
    } else {
        /* Convert a special key into a string of the form '(NAME)' */
        const char *name = "(?)";
        switch (ch) {
        case KEY_DOWN:          name = "(DOWN)"; break;
        case KEY_UP:            name = "(UP)"; break;
        case KEY_LEFT:          name = "(LEFT)"; break;
        case KEY_RIGHT:         name = "(RIGHT)"; break;
        case KEY_BACKSPACE:     name = "\b"; break;
        case KEY_F0:            name = "(F0)"; break;
        case KEY_F(1):          name = "(F1)"; break;
        case KEY_F(2):          name = "(F2)"; break;
        case KEY_F(3):          name = "(F3)"; break;
        case KEY_F(4):          name = "(F4)"; break;
        case KEY_F(5):          name = "(F5)"; break;
        case KEY_F(6):          name = "(F6)"; break;
        case KEY_F(7):          name = "(F7)"; break;
        case KEY_F(8):          name = "(F8)"; break;
        case KEY_F(9):          name = "(F9)"; break;
        case KEY_F(10):         name = "(F10)"; break;
        case KEY_F(11):         name = "(F11)"; break;
        case KEY_F(12):         name = "(F12)"; break;
        case KEY_F(13):         name = "(F13)"; break;
        case KEY_F(14):         name = "(F14)"; break;
        case KEY_F(15):         name = "(F15)"; break;
        case KEY_F(16):         name = "(F16)"; break;
        case KEY_F(17):         name = "(F17)"; break;
        case KEY_F(18):         name = "(F18)"; break;
        case KEY_F(19):         name = "(F19)"; break;
        case KEY_F(20):         name = "(F20)"; break;
        case KEY_F(21):         name = "(F21)"; break;
        case KEY_F(22):         name = "(F22)"; break;
        case KEY_F(23):         name = "(F23)"; break;
        case KEY_F(24):         name = "(F24)"; break;
        case KEY_DC:            name = "(DEL)"; break;
        case KEY_IC:            name = "(INS)"; break;
        case KEY_NPAGE:         name = "(PGUP)"; break;
        case KEY_PPAGE:         name = "(PGDN)"; break;
        case KEY_HOME:          name = "(HOME)"; break;
        case KEY_END:           name = "(END)"; break;
        case KEY_BTAB:          name = "(BTAB)"; break;
        case KEY_ENTER:         name = "\n"; break;
        default:                break;
        }
        str = ip_string_create(name);
    }
    ip_value_set_string(&(exec->this_value), str);
    ip_string_deref(str);
    return IP_EXEC_OK;
}

/**
 * @brief Deletes the character under the cursor.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_delete_char
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    (void)args;
    (void)num_args;
    init_screen(exec);
    delch();
    refresh();
    return IP_EXEC_OK;
}

/**
 * @brief Deletes the line under the cursor.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_delete_line
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    (void)args;
    (void)num_args;
    init_screen(exec);
    deleteln();
    refresh();
    return IP_EXEC_OK;
}

/**
 * @brief Inserts a string at the current cursor position.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_insert_string
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    ip_string_t *str = 0;
    if (num_args > 0) {
        status = ip_value_to_string(&(args[0]));
        if (status == IP_EXEC_OK) {
            str = args[0].svalue;
        }
    } else {
        status = ip_value_to_string(&(exec->this_value));
        if (status == IP_EXEC_OK) {
            str = exec->this_value.svalue;
        }
    }
    if (status == IP_EXEC_OK && str) {
        init_screen(exec);
        insstr(str->data);
        refresh();
    }
    return IP_EXEC_OK;
}

/**
 * @brief Inserts a new line under the cursor.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_insert_line
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    (void)args;
    (void)num_args;
    init_screen(exec);
    insertln();
    refresh();
    return IP_EXEC_OK;
}

/**
 * @brief Hides the cursor.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_hide_cursor
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    (void)args;
    (void)num_args;
    init_screen(exec);
    curs_set(0);
    refresh();
    return IP_EXEC_OK;
}

/**
 * @brief Shows the cursor.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_show_cursor
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    (void)args;
    (void)num_args;
    init_screen(exec);
    curs_set(1);
    refresh();
    return IP_EXEC_OK;
}

static ip_builtin_info_t const curses_builtins[] = {
    {"CLEAR SCREEN",                ip_clear_screen,        0,  0},
    {"CLEAR TO END OF SCREEN",      ip_clear_eos,           0,  0},
    {"CLEAR TO END OF LINE",        ip_clear_eol,           0,  0},
    {"MOVE CURSOR TO",              ip_move_cursor_to,      2,  2},
    {"CURSOR X",                    ip_cursor_x,            0,  0},
    {"CURSOR Y",                    ip_cursor_y,            0,  0},
    {"SCREEN WIDTH",                ip_screen_width,        0,  0},
    {"SCREEN HEIGHT",               ip_screen_height,       0,  0},
    {"NORMAL TEXT",                 ip_normal_text,         0,  0},
    {"BOLD TEXT",                   ip_bold_text,           0,  0},
    {"TEXT COLOR",                  ip_text_color,          1,  2},
    {"BEEP SPEAKER",                ip_beep,                0,  0},
    {"FLASH SCREEN",                ip_flash,               0,  0},
    {"INPUT KEY",                   ip_input_key,           0,  1},
    {"DELETE CHARACTER",            ip_delete_char,         0,  0},
    {"DELETE LINE",                 ip_delete_line,         0,  0},
    {"INSERT STRING",               ip_insert_string,       0,  1},
    {"INSERT LINE",                 ip_insert_line,         0,  0},
    {"HIDE CURSOR",                 ip_hide_cursor,         0,  0},
    {"SHOW CURSOR",                 ip_show_cursor,         0,  0},
    {0,                             0,                      0,  0}
};

static void ip_create_color(ip_program_t *program, const char *name, int value)
{
    ip_var_t *var = ip_var_create(&(program->vars), name, IP_TYPE_INT);
    if (var) {
        var->ivalue = value;
        var->base.flags |= IP_SYMBOL_DEFINED | IP_SYMBOL_NO_RESET;
    }
}

void ip_register_console_builtins(ip_program_t *program, unsigned options)
{
    if ((options & ITOK_TYPE_EXTENSION) != 0) {
        ip_program_register_builtins(program, curses_builtins);

        /* Create global variables for the standard text colors */
        ip_create_color(program, "BLACK", COLOR_BLACK);
        ip_create_color(program, "RED", COLOR_RED);
        ip_create_color(program, "GREEN", COLOR_GREEN);
        ip_create_color(program, "YELLOW", COLOR_YELLOW);
        ip_create_color(program, "BLUE", COLOR_BLUE);
        ip_create_color(program, "MAGENTA", COLOR_MAGENTA);
        ip_create_color(program, "CYAN", COLOR_CYAN);
        ip_create_color(program, "WHITE", COLOR_WHITE);
    }
}
