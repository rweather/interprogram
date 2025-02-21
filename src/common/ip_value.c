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

#include "ip_value.h"
#include "ip_exec.h"

void ip_value_init(ip_value_t *value)
{
    value->type = IP_TYPE_UNKNOWN;
}

void ip_value_release(ip_value_t *value)
{
    value->type = IP_TYPE_UNKNOWN;
}

void ip_value_assign(ip_value_t *dest, const ip_value_t *src)
{
    if (dest != src) {
        *dest = *src;
    }
}

void ip_value_set_int(ip_value_t *dest, ip_int_t src)
{
    dest->type = IP_TYPE_INT;
    dest->ivalue = src;
}

void ip_value_set_float(ip_value_t *dest, ip_float_t src)
{
    dest->type = IP_TYPE_FLOAT;
    dest->fvalue = src;
}

void ip_value_set_unknown(ip_value_t *value)
{
    value->type = IP_TYPE_UNKNOWN;
}

int ip_value_to_int(ip_value_t *value)
{
    switch (value->type) {
    case IP_TYPE_INT: break;

    case IP_TYPE_FLOAT:
        value->fvalue = (ip_float_t)(value->ivalue);
        value->type = IP_TYPE_INT;
        break;

    default:
        return IP_EXEC_BAD_TYPE;
    }
    return IP_EXEC_OK;
}

int ip_value_to_float(ip_value_t *value)
{
    switch (value->type) {
    case IP_TYPE_INT:
        value->ivalue = (ip_int_t)(value->fvalue);
        value->type = IP_TYPE_FLOAT;
        break;

    case IP_TYPE_FLOAT: break;

    default:
        return IP_EXEC_BAD_TYPE;
    }
    return IP_EXEC_OK;
}

int ip_value_from_var(ip_value_t *dest, const ip_var_t *src)
{
    switch (src->type) {
    case IP_TYPE_INT:
        dest->type = IP_TYPE_INT;
        dest->ivalue = src->ivalue;
        break;

    case IP_TYPE_FLOAT:
        dest->type = IP_TYPE_FLOAT;
        dest->fvalue = src->fvalue;
        break;

    case IP_TYPE_ARRAY_OF_INT:
    default:
        dest->type = IP_TYPE_INT;
        dest->ivalue = 0;
        return IP_EXEC_BAD_TYPE;

    case IP_TYPE_ARRAY_OF_FLOAT:
        dest->type = IP_TYPE_FLOAT;
        dest->fvalue = 0;
        return IP_EXEC_BAD_TYPE;
    }
    if (src->initialised) {
        return IP_EXEC_OK;
    } else {
        return IP_EXEC_UNINIT;
    }
}

int ip_value_to_var(ip_var_t *dest, const ip_value_t *src)
{
    switch (dest->type) {
    case IP_TYPE_INT:
        if (src->type == IP_TYPE_INT) {
            dest->ivalue = src->ivalue;
            dest->initialised = 1;
        } else if (src->type == IP_TYPE_FLOAT) {
            dest->ivalue = (ip_int_t)(src->fvalue);
            dest->initialised = 1;
        } else {
            return IP_EXEC_BAD_TYPE;
        }
        break;

    case IP_TYPE_FLOAT:
        if (src->type == IP_TYPE_FLOAT) {
            dest->fvalue = src->fvalue;
            dest->initialised = 1;
        } else if (src->type == IP_TYPE_INT) {
            dest->fvalue = (ip_float_t)(src->ivalue);
            dest->initialised = 1;
        } else {
            return IP_EXEC_BAD_TYPE;
        }
        break;

    default:
        return IP_EXEC_BAD_TYPE;
    }
    return IP_EXEC_OK;
}

static int ip_value_validate_index(const ip_var_t *src, ip_int_t index)
{
    return (index >= src->min_subscript && index <= src->max_subscript);
}

int ip_value_from_array(ip_value_t *dest, const ip_var_t *src, ip_int_t index)
{
    switch (src->type) {
    case IP_TYPE_INT:
    default:
        dest->type = IP_TYPE_INT;
        dest->ivalue = 0;
        return IP_EXEC_BAD_TYPE;

    case IP_TYPE_FLOAT:
        dest->type = IP_TYPE_FLOAT;
        dest->fvalue = 0;
        return IP_EXEC_BAD_TYPE;

    case IP_TYPE_ARRAY_OF_INT:
        dest->type = IP_TYPE_INT;
        if (ip_value_validate_index(src, index)) {
            dest->ivalue = src->iarray[index - src->min_subscript];
        } else {
            dest->ivalue = 0;
            return IP_EXEC_BAD_INDEX;
        }
        break;

    case IP_TYPE_ARRAY_OF_FLOAT:
        dest->type = IP_TYPE_FLOAT;
        if (ip_value_validate_index(src, index)) {
            dest->fvalue = src->farray[index - src->min_subscript];
        } else {
            dest->fvalue = 0;
            return IP_EXEC_BAD_INDEX;
        }
        break;
    }
    return IP_EXEC_OK;
}

int ip_value_to_array(ip_var_t *dest, ip_int_t index, const ip_value_t *src)
{
    switch (dest->type) {
    case IP_TYPE_ARRAY_OF_INT:
        if (!ip_value_validate_index(dest, index)) {
            return IP_EXEC_BAD_INDEX;
        }
        if (src->type == IP_TYPE_INT) {
            dest->iarray[index - dest->min_subscript] = src->ivalue;
        } else if (src->type == IP_TYPE_FLOAT) {
            dest->iarray[index - dest->min_subscript] = (ip_int_t)(src->fvalue);
        } else {
            return IP_EXEC_BAD_TYPE;
        }
        break;

    case IP_TYPE_ARRAY_OF_FLOAT:
        if (!ip_value_validate_index(dest, index)) {
            return IP_EXEC_BAD_INDEX;
        }
        if (src->type == IP_TYPE_FLOAT) {
            dest->farray[index - dest->min_subscript] = src->fvalue;
        } else if (src->type == IP_TYPE_INT) {
            dest->farray[index - dest->min_subscript] = (ip_float_t)(src->ivalue);
        } else {
            return IP_EXEC_BAD_TYPE;
        }
        break;

    default:
        return IP_EXEC_BAD_TYPE;
    }
    return IP_EXEC_OK;
}
