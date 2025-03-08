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

int ip_bitwise_and(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)num_args;
    status = ip_value_to_int(&(args[0]));
    if (status == IP_EXEC_OK) {
        status = ip_value_to_int(&(exec->this_value));
        if (status == IP_EXEC_OK) {
            ip_value_set_int
                (&(exec->this_value),
                 exec->this_value.ivalue & args[0].ivalue);
        }
    }
    return status;
}

int ip_bitwise_and_not(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)num_args;
    status = ip_value_to_int(&(args[0]));
    if (status == IP_EXEC_OK) {
        status = ip_value_to_int(&(exec->this_value));
        if (status == IP_EXEC_OK) {
            ip_value_set_int
                (&(exec->this_value),
                 exec->this_value.ivalue & ~(args[0].ivalue));
        }
    }
    return status;
}

int ip_bitwise_or(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)num_args;
    status = ip_value_to_int(&(args[0]));
    if (status == IP_EXEC_OK) {
        status = ip_value_to_int(&(exec->this_value));
        if (status == IP_EXEC_OK) {
            ip_value_set_int
                (&(exec->this_value),
                 exec->this_value.ivalue | args[0].ivalue);
        }
    }
    return status;
}

int ip_bitwise_xor(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)num_args;
    status = ip_value_to_int(&(args[0]));
    if (status == IP_EXEC_OK) {
        status = ip_value_to_int(&(exec->this_value));
        if (status == IP_EXEC_OK) {
            ip_value_set_int
                (&(exec->this_value),
                 exec->this_value.ivalue ^ args[0].ivalue);
        }
    }
    return status;
}

int ip_bitwise_not(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)args;
    (void)num_args;
    status = ip_value_to_int(&(exec->this_value));
    if (status == IP_EXEC_OK) {
        ip_value_set_float
            (&(exec->this_value), ~(exec->this_value.ivalue));
    }
    return status;
}

static ip_int_t ip_shift_by(ip_int_t value, ip_int_t n)
{
    if (n >= (ip_int_t)(sizeof(ip_int_t) * 8)) {
        /* Shifted left so far that the result is zero */
        return 0;
    } else if (n >= 0) {
        /* Regular shift left */
        return value << n;
    } else if (n <= -((ip_int_t)(sizeof(ip_int_t) * 8))) {
        /* Shifted right so far that the result is zero or -1 */
        return value < 0 ? -1 : 0;
    } else {
        /* Regular shift right */
        return value >> (-n);
    }
}

int ip_shift_left(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)num_args;
    status = ip_value_to_int(&(args[0]));
    if (status == IP_EXEC_OK) {
        status = ip_value_to_int(&(exec->this_value));
        if (status == IP_EXEC_OK) {
            ip_value_set_int
                (&(exec->this_value),
                 ip_shift_by(exec->this_value.ivalue, args[0].ivalue));
        }
    }
    return status;
}

int ip_shift_right(ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)num_args;
    status = ip_value_to_int(&(args[0]));
    if (status == IP_EXEC_OK) {
        status = ip_value_to_int(&(exec->this_value));
        if (status == IP_EXEC_OK) {
            ip_value_set_int
                (&(exec->this_value),
                 ip_shift_by(exec->this_value.ivalue, -(args[0].ivalue)));
        }
    }
    return status;
}
