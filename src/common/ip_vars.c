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

#include "ip_vars.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void ip_var_init_string_array(ip_string_t **array, ip_uint_t size)
{
    ip_string_t *str = ip_string_create_empty();
    while (size > 0) {
        ip_string_ref(str);
        *array++ = str;
        --size;
    }
    ip_string_deref(str);
}

static void ip_var_free_string_array(ip_string_t **array, ip_uint_t size)
{
    while (size > 0) {
        ip_string_deref(*array++);
        --size;
    }
}

static void ip_var_free(ip_symbol_t *symbol)
{
    ip_var_t *var = (ip_var_t *)symbol;
    switch (ip_var_get_type(var)) {
    case IP_TYPE_STRING:
        ip_string_deref(var->svalue);
        break;

    case IP_TYPE_ARRAY_OF_INT:
        if (var->iarray) {
            free(var->iarray);
        }
        break;

    case IP_TYPE_ARRAY_OF_FLOAT:
        if (var->farray) {
            free(var->farray);
        }
        break;

    case IP_TYPE_ARRAY_OF_STRING:
        if (var->sarray) {
            ip_var_free_string_array
                (var->sarray, var->max_subscript - var->min_subscript + 1);
            free(var->sarray);
        }
        break;

    default: break;
    }
}

static void ip_var_reset(ip_symbol_t *symbol)
{
    ip_var_t *var = (ip_var_t *)symbol;
    ip_uint_t size;
    switch (ip_var_get_type(var)) {
    case IP_TYPE_INT:
        ip_var_mark_as_uninitialised(var);
        var->ivalue = 0;
        break;

    case IP_TYPE_FLOAT:
        ip_var_mark_as_uninitialised(var);
        var->fvalue = 0;
        break;

    case IP_TYPE_STRING:
        ip_var_mark_as_uninitialised(var);
        if (var->svalue->len != 0) {
            ip_string_deref(var->svalue);
            var->svalue = ip_string_create_empty();
        }
        break;

    case IP_TYPE_ARRAY_OF_INT:
        size = var->max_subscript - var->min_subscript + 1;
        memset(var->iarray, 0, sizeof(ip_int_t) * size);
        break;

    case IP_TYPE_ARRAY_OF_FLOAT:
        size = var->max_subscript - var->min_subscript + 1;
        memset(var->farray, 0, sizeof(ip_float_t) * size);
        break;

    case IP_TYPE_ARRAY_OF_STRING:
        size = var->max_subscript - var->min_subscript + 1;
        ip_var_free_string_array(var->sarray, size);
        ip_var_init_string_array(var->sarray, size);
        break;

    default: break;
    }
}

void ip_var_table_init(ip_var_table_t *vars)
{
    ip_symbol_table_init(&(vars->symbols));
    vars->symbols.free_symbol = ip_var_free;
    vars->symbols.reset_symbol = ip_var_reset;
}

void ip_var_table_free(ip_var_table_t *vars)
{
    ip_symbol_table_free(&(vars->symbols));
}

void ip_var_table_reset(ip_var_table_t *vars)
{
    ip_symbol_table_reset(&(vars->symbols));
}

ip_var_t *ip_var_lookup(const ip_var_table_t *vars, const char *name)
{
    return (ip_var_t *)ip_symbol_lookup_by_name(&(vars->symbols), name);
}

ip_var_t *ip_var_create
    (ip_var_table_t *vars, const char *name, unsigned char type)
{
    ip_var_t *var;

    /* Bail out if the variable is already in the tree */
    var = ip_var_lookup(vars, name);
    if (var) {
        return 0;
    }

    /* Construct a new variable node and give it a name and type */
    var = calloc(1, sizeof(ip_var_t));
    if (var) {
        var->base.name = strdup(name);
        if (!(var->base.name)) {
            ip_out_of_memory();
        }
        var->base.num = -1;
    } else {
        ip_out_of_memory();
    }
    var->base.type = type;
    if (type == IP_TYPE_STRING) {
        var->svalue = ip_string_create_empty();
    }

    /* Insert the new variable into the red-black tree */
    ip_symbol_insert(&(vars->symbols), &(var->base));
    return var;
}

void ip_var_dimension_array
    (ip_var_t *var, ip_int_t min_subscript, ip_int_t max_subscript)
{
    ip_uint_t prev_size;
    ip_uint_t size;

    /* Check that the variable can be converted into an array */
    if (var->base.type < IP_TYPE_INT ||
            var->base.type > IP_TYPE_ARRAY_OF_STRING) {
        return;
    }

    /* Determine the size of the array from the subscript range */
    prev_size = var->max_subscript - var->min_subscript + 1;
    size = max_subscript - min_subscript + 1;
    var->min_subscript = min_subscript;
    var->max_subscript = max_subscript;
    ip_var_mark_as_initialised(var); /* Arrays are implicitly initialised */

    /* Convert the variable into an array if it isn't already */
    if (var->base.type == IP_TYPE_INT) {
        var->base.type = IP_TYPE_ARRAY_OF_INT;
        var->iarray = 0;
    } else if (var->base.type == IP_TYPE_FLOAT) {
        var->base.type = IP_TYPE_ARRAY_OF_FLOAT;
        var->farray = 0;
    } else if (var->base.type == IP_TYPE_STRING) {
        var->base.type = IP_TYPE_ARRAY_OF_STRING;
        var->sarray = 0;
    }

    /* Allocate or reallocate the memory for the array */
    if (var->base.type == IP_TYPE_ARRAY_OF_INT) {
        if (!(var->iarray)) {
            var->iarray = malloc(size * sizeof(ip_int_t));
        } else if (size != prev_size) {
            var->iarray = realloc(var->iarray, size * sizeof(ip_int_t));
        }
        if (!(var->iarray)) {
            ip_out_of_memory();
        }
        memset(var->iarray, 0, size * sizeof(ip_int_t));
    } else if (var->base.type == IP_TYPE_ARRAY_OF_FLOAT) {
        if (!(var->farray)) {
            var->farray = malloc(size * sizeof(ip_float_t));
        } else if (size != prev_size) {
            var->farray = realloc(var->farray, size * sizeof(ip_float_t));
        }
        if (!(var->farray)) {
            ip_out_of_memory();
        }
        memset(var->farray, 0, size * sizeof(ip_float_t));
    } else {
        if (var->sarray) {
            ip_var_free_string_array(var->sarray, size);
            if (size != prev_size) {
                var->sarray = realloc(var->sarray, size * sizeof(ip_string_t));
            }
        } else {
            var->sarray = malloc(size * sizeof(ip_string_t *));
        }
        if (!(var->sarray)) {
            ip_out_of_memory();
        }
        ip_var_init_string_array(var->sarray, size);
    }
}

int ip_var_is_array(const ip_var_t *var)
{
    if (!var) {
        return 0;
    }
    return var->base.type == IP_TYPE_ARRAY_OF_INT ||
           var->base.type == IP_TYPE_ARRAY_OF_FLOAT ||
           var->base.type == IP_TYPE_ARRAY_OF_STRING;
}
