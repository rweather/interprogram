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
#include <math.h>

/* Classic INTERPROGRAM specifies angles in multiplies of pi, which is the
 * purpose of the "pis" functions below.  Extended INTERPROGRAM uses radians. */

int ip_sin_pis(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), sin(exec->this_value.fvalue * M_PI));
    }
    return status;
}

int ip_sin_radians(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), sin(exec->this_value.fvalue));
    }
    return status;
}

int ip_cos_pis(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), cos(exec->this_value.fvalue * M_PI));
    }
    return status;
}

int ip_cos_radians(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), cos(exec->this_value.fvalue));
    }
    return status;
}

int ip_tan_pis(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), tan(exec->this_value.fvalue * M_PI));
    }
    return status;
}

int ip_tan_radians(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), tan(exec->this_value.fvalue));
    }
    return status;
}

int ip_atan_pis(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), atan(exec->this_value.fvalue) / M_PI);
    }
    return status;
}

int ip_atan_radians(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), atan(exec->this_value.fvalue));
    }
    return status;
}
