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

/* Reference for red-black trees: Algorithms in C++, Robert Sedgwick, 1992 */

void ip_var_table_init(ip_var_table_t *vars)
{
    memset(vars, 0, sizeof(ip_var_table_t));
    vars->nil.left = &(vars->nil);
    vars->nil.right = &(vars->nil);
    vars->root.right = &(vars->nil);
}

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

static void ip_var_free(ip_var_table_t *vars, ip_var_t *var)
{
    if (var && var != &(vars->nil)) {
        if (var->name) {
            free(var->name);
        }
        switch (var->type) {
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
        ip_var_free(vars, var->left);
        ip_var_free(vars, var->right);
        free(var);
    }
}

void ip_var_table_free(ip_var_table_t *vars)
{
    ip_var_free(vars, vars->root.right);
    memset(vars, 0, sizeof(ip_var_table_t));
}

static void ip_var_reset(ip_var_table_t *vars, ip_var_t *var)
{
    ip_uint_t size;
    if (var != &(vars->nil)) {
        if (!(var->not_resettable)) {
            switch (var->type) {
            case IP_TYPE_INT:
                var->initialised = 0;
                var->ivalue = 0;
                break;

            case IP_TYPE_FLOAT:
                var->initialised = 0;
                var->fvalue = 0;
                break;

            case IP_TYPE_STRING:
                var->initialised = 0;
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
        ip_var_reset(vars, var->left);
        ip_var_reset(vars, var->right);
    }
}

void ip_var_table_reset(ip_var_table_t *vars)
{
    ip_var_reset(vars, vars->root.right);
}

ip_var_t *ip_var_lookup(const ip_var_table_t *vars, const char *name)
{
    ip_var_t *var = vars->root.right;
    int cmp;
    while (var != &(vars->nil)) {
        cmp = var->name ? strcmp(name, var->name) : 1;
        if (cmp == 0) {
            return var;
        } else if (cmp < 0) {
            var = var->left;
        } else {
            var = var->right;
        }
    }
    return 0;
}

/**
 * @brief Rotates the nodes in a red-black tree to rebalance it.
 *
 * @param[in] node The node that is being inserted into the tree.
 * @param[in] y The node to rotate around.
 */
static ip_var_t *ip_var_rotate(ip_var_t *node, ip_var_t *y)
{
    ip_var_t *c;
    ip_var_t *gc;
    int cmp, cmp2;
    cmp = y->name ? strcmp(node->name, y->name) : 1;
    if (cmp < 0) {
        c = y->left;
    } else {
        c = y->right;
    }
    cmp2 = c->name ? strcmp(node->name, c->name) : 1;
    if (cmp2 < 0) {
        gc = c->left;
        c->left = gc->right;
        gc->right = c;
    } else {
        gc = c->right;
        c->right = gc->left;
        gc->left = c;
    }
    if (cmp < 0) {
        y->left = gc;
    } else {
        y->right = gc;
    }
    return gc;
}

/**
 * @brief Insert a node into a red-black tree.
 *
 * @param[in,out] vars The red-black tree to insert into.
 * @param[in] node The node to insert.  The name must not already be
 * present in the tree.
 */
static void ip_var_insert(ip_var_table_t *vars, ip_var_t *node)
{
    ip_var_t *x = &(vars->root);
    ip_var_t *p = x;
    ip_var_t *g = x;
    ip_var_t *gg = &(vars->nil);
    int cmp, cmp2;

    /* Prepare the node for insertion */
    node->red = 1;
    node->left = &(vars->nil);
    node->right = &(vars->nil);

    /* Find the place to insert the node, rearranging the tree as needed */
    while (x != &(vars->nil)) {
        gg = g;
        g = p;
        p = x;
        cmp = x->name ? strcmp(node->name, x->name) : 1;
        if (cmp < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
        if (x->left->red && x->right->red) {
            x->red = 1;
            x->left->red = 0;
            x->right->red = 0;
            if (p->red) {
                g->red = 1;
                cmp  = g->name ? strcmp(node->name, g->name) : 1;
                cmp2 = p->name ? strcmp(node->name, p->name) : 1;
                if (cmp != cmp2) {
                    p = ip_var_rotate(node, g);
                }
                x = ip_var_rotate(node, gg);
                x->red = 0;
            }
        }
    }

    /* Insert the node into the tree */
    x = node;
    cmp = p->name ? strcmp(x->name, p->name) : 1;
    if (cmp < 0) {
        p->left = x;
    } else {
        p->right = x;
    }
    x->red = 1;
    x->left->red = 0;
    x->right->red = 0;
    if (p->red) {
        g->red = 1;
        cmp  = g->name ? strcmp(node->name, g->name) : 1;
        cmp2 = p->name ? strcmp(node->name, p->name) : 1;
        if (cmp != cmp2) {
            p = ip_var_rotate(node, g);
        }
        x = ip_var_rotate(node, gg);
        x->red = 0;
    }
    vars->root.right->red = 0;
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
        var->name = strdup(name);
    }
    if (!var || !(var->name)) {
        ip_var_free(vars, var);
        ip_out_of_memory();
    }
    var->type = type;
    if (var->type == IP_TYPE_STRING) {
        var->svalue = ip_string_create_empty();
    }

    /* Insert the new variable into the red-black tree */
    ip_var_insert(vars, var);
    return var;
}

void ip_var_dimension_array
    (ip_var_t *var, ip_int_t min_subscript, ip_int_t max_subscript)
{
    ip_uint_t prev_size;
    ip_uint_t size;

    /* Check that the variable can be converted into an array */
    if (var->type < IP_TYPE_INT || var->type > IP_TYPE_ARRAY_OF_STRING) {
        return;
    }

    /* Determine the size of the array from the subscript range */
    prev_size = var->max_subscript - var->min_subscript + 1;
    size = max_subscript - min_subscript + 1;
    var->min_subscript = min_subscript;
    var->max_subscript = max_subscript;
    var->initialised = 1; /* Arrays are implicitly initialised */

    /* Convert the variable into an array if it isn't already */
    if (var->type == IP_TYPE_INT) {
        var->type = IP_TYPE_ARRAY_OF_INT;
        var->iarray = 0;
    } else if (var->type == IP_TYPE_FLOAT) {
        var->type = IP_TYPE_ARRAY_OF_FLOAT;
        var->farray = 0;
    } else if (var->type == IP_TYPE_STRING) {
        var->type = IP_TYPE_ARRAY_OF_STRING;
        var->sarray = 0;
    }

    /* Allocate or reallocate the memory for the array */
    if (var->type == IP_TYPE_ARRAY_OF_INT) {
        if (!(var->iarray)) {
            var->iarray = malloc(size * sizeof(ip_int_t));
        } else if (size != prev_size) {
            var->iarray = realloc(var->iarray, size * sizeof(ip_int_t));
        }
        if (!(var->iarray)) {
            ip_out_of_memory();
        }
        memset(var->iarray, 0, size * sizeof(ip_int_t));
    } else if (var->type == IP_TYPE_ARRAY_OF_FLOAT) {
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
    return var->type == IP_TYPE_ARRAY_OF_INT ||
           var->type == IP_TYPE_ARRAY_OF_FLOAT ||
           var->type == IP_TYPE_ARRAY_OF_STRING;
}
