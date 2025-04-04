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

int ip_sqrt(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), sqrt(exec->this_value.fvalue));
    }
    return status;
}

int ip_sqrt_of(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)exec;
    (void)num_args;
    status = ip_value_to_float(&(args[0]));
    if (status == IP_EXEC_OK) {
        ip_value_set_float(&(args[0]), sqrt(args[0].fvalue));
    }
    return status;
}

int ip_cube_root(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), cbrt(exec->this_value.fvalue));
    }
    return status;
}

int ip_cube_root_of(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)exec;
    (void)num_args;
    status = ip_value_to_float(&(args[0]));
    if (status == IP_EXEC_OK) {
        ip_value_set_float(&(args[0]), cbrt(args[0].fvalue));
    }
    return status;
}

int ip_log(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), log(exec->this_value.fvalue));
    }
    return status;
}

int ip_log_base(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        status = ip_value_to_float(&(args[0]));
        if (status == IP_EXEC_OK) {
            ip_float_t value;
            if (args[0].fvalue == 10.0) {
                value = log10(exec->this_value.fvalue);
            } else if (args[0].fvalue == 2.0) {
                value = log2(exec->this_value.fvalue);
            } else if (fabs(args[0].fvalue) >= IP_FLOAT_EPSILON) {
                value = log(exec->this_value.fvalue) / log(args[0].fvalue);
            } else {
                return IP_EXEC_DIV_ZERO;
            }
            ip_value_set_float(&(exec->this_value), value);
        }
    }
    return status;
}

int ip_log_of(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)exec;
    (void)num_args;
    status = ip_value_to_float(&(args[0]));
    if (status == IP_EXEC_OK) {
        ip_value_set_float(&(args[0]), log(args[0].fvalue));
    }
    return status;
}

int ip_exp(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_float(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), exp(exec->this_value.fvalue));
    }
    return status;
}

int ip_exp_of(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)exec;
    (void)num_args;
    status = ip_value_to_float(&(args[0]));
    if (status == IP_EXEC_OK) {
        ip_value_set_float(&(args[0]), exp(args[0].fvalue));
    }
    return status;
}

int ip_pow(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)num_args;
    status = ip_value_to_float(&(args[0]));
    if (status == IP_EXEC_OK) {
        status = ip_value_to_float(&(exec->this_value));
        if (status == IP_EXEC_OK) {
            ip_value_set_float
                (&(exec->this_value),
                 pow(exec->this_value.fvalue, args[0].fvalue));
        }
    }
    return status;
}
