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

int ip_abs(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    if (exec->this_value.type == IP_TYPE_INT) {
        if (exec->this_value.ivalue < 0) {
            exec->this_value.ivalue = -(exec->this_value.ivalue);
        }
    } else {
        status = ip_value_to_float(&(exec->this_value));
        if (status == IP_EXEC_OK) {
            if (exec->this_value.fvalue < 0) {
                exec->this_value.fvalue = -(exec->this_value.fvalue);
            }
        }
    }
    return status;
}

int ip_abs_of(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)exec;
    (void)num_args;
    if (args[0].type == IP_TYPE_INT) {
        if (args[0].ivalue < 0) {
            args[0].ivalue = -(args[0].ivalue);
        }
    } else {
        status = ip_value_to_float(&(args[0]));
        if (status == IP_EXEC_OK) {
            if (args[0].fvalue < 0) {
                args[0].fvalue = -(args[0].fvalue);
            }
        }
    }
    return status;
}

int ip_rand(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    ip_float_t value = ((ip_float_t)rand()) / (((ip_float_t)RAND_MAX) + 1);
    (void)exec;
    (void)num_args;
    ip_value_set_float(&(args[0]), value);
    return IP_EXEC_OK;
}

int ip_srand(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)exec;
    (void)num_args;
    status = ip_value_to_int(&(args[0]));
    if (status == IP_EXEC_OK) {
        srand((unsigned)(args[0].ivalue));
    }
    return status;
}
