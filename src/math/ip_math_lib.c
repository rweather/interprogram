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

#include "ip_math_lib.h"
#include <stdlib.h>
#include <time.h>

static ip_builtin_info_t const classic_math_builtins[] = {
    /* Classic INTERPROGRAM's trig functions take angles in units of pi */
    {"FORM SINE",                   ip_sin_pis,             0,  0},
    {"FORM COSINE",                 ip_cos_pis,             0,  0},
    {"FORM TANGENT",                ip_tan_pis,             0,  0},
    {"FORM ARCTAN",                 ip_atan_pis,            0,  0},
    {"FORM SQUARE ROOT",            ip_sqrt,                0,  0},
    {"FORM NATURAL LOG",            ip_log,                 0,  0},
    {"FORM EXPONENTIAL",            ip_exp,                 0,  0},
    {0,                             0,                      0,  0}
};

static ip_builtin_info_t const extension_math_builtins[] = {
    /* Extended INTERPROGRAM's trig functions take angles in radians */
    {"FORM SINE",                   ip_sin_radians,         0,  0},
    {"FORM COSINE",                 ip_cos_radians,         0,  0},
    {"FORM TANGENT",                ip_tan_radians,         0,  0},
    {"FORM ARCTAN",                 ip_atan_radians,        0,  0},
    {"FORM SQUARE ROOT",            ip_sqrt,                0,  0},
    {"FORM NATURAL LOG",            ip_log,                 0,  0},
    {"FORM EXPONENTIAL",            ip_exp,                 0,  0},
    {"FORM ABSOLUTE VALUE",         ip_abs,                 0,  0},
    {"RAISE TO THE POWER OF",       ip_pow,                 1,  1},
    {"ROUND NEAREST",               ip_round_nearest,       0,  0},
    {"ROUND DOWN",                  ip_round_down,          0,  0},
    {"ROUND UP",                    ip_round_up,            0,  0},
    {"ROUND TO A MULTIPLE OF",      ip_round_multiple,      1,  1},
    {"BITWISE AND WITH",            ip_bitwise_and,         1,  1},
    {"BITWISE AND WITH NOT",        ip_bitwise_and_not,     1,  1},
    {"BITWISE OR WITH",             ip_bitwise_or,          1,  1},
    {"BITWISE XOR WITH",            ip_bitwise_xor,         1,  1},
    {"BITWISE NOT",                 ip_bitwise_not,         0,  0},
    {"SHIFT LEFT BY",               ip_shift_left,          1,  1},
    {"SHIFT RIGHT BY",              ip_shift_right,         1,  1},
    {"RANDOM NUMBER",               ip_rand,                0,  0},
    {"SEED RANDOM",                 ip_srand,               1,  1},

    /* Functions that can be used in expressions.  The minimum number of
     * arguments must be greater than the maximum to form a function. */
    {"SINE OF",                     ip_sin_of,              1,  0},
    {"COSINE OF",                   ip_cos_of,              1,  0},
    {"TANGENT OF",                  ip_tan_of,              1,  0},
    {"ARCTAN OF",                   ip_atan_of,             1,  0},
    {"SQUARE ROOT OF",              ip_sqrt_of,             1,  0},
    {"NATURAL LOG OF",              ip_log_of,              1,  0},
    {"EXPONENTIAL OF",              ip_exp_of,              1,  0},
    {"ABSOLUTE VALUE OF",           ip_abs_of,              1,  0},

    /* End of the list */
    {0,                             0,                      0,  0}
};

void ip_register_math_builtins(ip_program_t *program, unsigned options)
{
    /* Register the built-ins for the selected language variant */
    if ((options & ITOK_TYPE_EXTENSION) != 0) {
        ip_program_register_builtins(program, extension_math_builtins);
    } else {
        ip_program_register_builtins(program, classic_math_builtins);
    }

    /* Seed the random number generator at startup */
    srand(time(0));
}
