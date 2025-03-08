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

#include "ip_string_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

/**
 * @brief Trims spaces from the start and end of the string in "THIS".
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_trim_string
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status = IP_EXEC_OK;
    ip_string_t *str;
    size_t posn, len;
    (void)args;
    (void)num_args;
    if (exec->this_value.type == IP_TYPE_STRING) {
        str = exec->this_value.svalue;
        posn = 0;
        len = str->len;
        while (posn < len && ip_char_is_whitespace(str->data[posn])) {
            ++posn;
        }
        while (len > posn && ip_char_is_whitespace(str->data[len - 1])) {
            --len;
        }
        str = ip_string_substring(str, posn, len - posn);
        ip_value_set_string(&(exec->this_value), str);
        ip_string_deref(str);
    } else {
        status = IP_EXEC_BAD_TYPE;
    }
    return status;
}

/**
 * @brief Pads the string in "THIS" on the left with extra spaces.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_pad_left
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)num_args;
    status = ip_value_to_int(&(args[0]));
    if (status == IP_EXEC_OK) {
        if (exec->this_value.type == IP_TYPE_STRING) {
            ip_string_t *str = exec->this_value.svalue;
            if (args[0].ivalue >= 0 &&
                    args[0].ivalue > (ip_int_t)(str->len)) {
                str = ip_string_pad_left
                    (str, args[0].ivalue - str->len);
                ip_value_set_string(&(exec->this_value), str);
                ip_string_deref(str);
            }
        } else {
            status = IP_EXEC_BAD_TYPE;
        }
    }
    return status;
}

/**
 * @brief Pads the string in "THIS" on the right with extra spaces.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_pad_right
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status;
    (void)num_args;
    status = ip_value_to_int(&(args[0]));
    if (status == IP_EXEC_OK) {
        if (exec->this_value.type == IP_TYPE_STRING) {
            ip_string_t *str = exec->this_value.svalue;
            if (args[0].ivalue >= 0 &&
                    args[0].ivalue > (ip_int_t)(str->len)) {
                str = ip_string_pad_right
                    (str, args[0].ivalue - str->len);
                ip_value_set_string(&(exec->this_value), str);
                ip_string_deref(str);
            }
        } else {
            status = IP_EXEC_BAD_TYPE;
        }
    }
    return status;
}

/**
 * @brief Converts the number in "THIS" into a string.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_number_to_string
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    char buffer[64];
    int status = IP_EXEC_OK;
    (void)args;
    (void)num_args;
    if (exec->this_value.type == IP_TYPE_INT) {
        snprintf(buffer, sizeof(buffer), "%" PRId64, exec->this_value.ivalue);
        ip_value_set_string(&(exec->this_value), ip_string_create(buffer));
    } else {
        status = ip_value_to_float(&(exec->this_value));
        if (status == IP_EXEC_OK) {
            snprintf(buffer, sizeof(buffer), "%g", exec->this_value.fvalue);
            ip_value_set_string(&(exec->this_value), ip_string_create(buffer));
        }
    }
    return status;
}

/**
 * @brief Converts the string in "THIS" into a floating-point value.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_string_to_number
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status = IP_EXEC_OK;
    status = ip_trim_string(exec, args, num_args);
    if (status == IP_EXEC_OK) {
        char *end;
        double value = strtod(exec->this_value.svalue->data, &end);
        if (end == exec->this_value.svalue->data || *end != '\0') {
            status = IP_EXEC_BAD_INPUT;
        } else {
            ip_value_set_float(&(exec->this_value), (ip_float_t)value);
        }
    }
    return status;
}

/**
 * @brief Converts the string in "THIS" into an integer value.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_string_to_integer
    (ip_exec_t *exec, ip_value_t *args, size_t num_args)
{
    int status = IP_EXEC_OK;
    int base = 10;
    if (num_args > 0) {
        /* Parse the first argument as a base */
        status = ip_value_to_int(&(args[0]));
        if (status != IP_EXEC_OK) {
            return status;
        }
        if (args[0].ivalue < 0 || args[0].ivalue == 1 || args[0].ivalue > 36) {
            return IP_EXEC_BAD_INPUT;
        }
        base = (int)(args[0].ivalue);
    }
    status = ip_trim_string(exec, args, num_args);
    if (status == IP_EXEC_OK) {
        char *end;
        long long value = strtoll(exec->this_value.svalue->data, &end, base);
        if (end == exec->this_value.svalue->data || *end != '\0') {
            status = IP_EXEC_BAD_INPUT;
        } else {
            ip_value_set_int(&(exec->this_value), (ip_int_t)value);
        }
    }
    return status;
}

static ip_builtin_info_t const string_builtins[] = {
    {"TRIM STRING",                 ip_trim_string,         0},
    {"PAD STRING ON LEFT",          ip_pad_left,            1},
    {"PAD STRING ON RIGHT",         ip_pad_right,           1},
    {"NUMBER TO STRING",            ip_number_to_string,    0},
    {"STRING TO NUMBER",            ip_string_to_number,    0},
    {"STRING TO INTEGER",           ip_string_to_integer,  -1},
    {0,                             0,                      0}
};

void ip_register_string_builtins(ip_program_t *program, unsigned options)
{
    if ((options & ITOK_TYPE_EXTENSION) != 0) {
        ip_program_register_builtins(program, string_builtins);
    }
}
