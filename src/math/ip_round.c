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

#include "ip_math.h"
#include <math.h>

int ip_round_nearest(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), round(exec->this_value.fvalue));
    }
    return status;
}

int ip_round_down(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), floor(exec->this_value.fvalue));
    }
    return status;
}

int ip_round_up(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), ceil(exec->this_value.fvalue));
    }
    return status;
}
