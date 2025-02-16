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

#ifndef INTERPROGRAM_VARS_H
#define INTERPROGRAM_VARS_H

#include "ip_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Information about a variable that is stored in the global
 * symbol lookup red-black tree.
 */
typedef struct ip_var_s ip_var_t;
struct ip_var_s
{
    /** Name of the variable */
    char *name;

    /** Type of the variable */
    unsigned char type;

    /** Non-zero once the variable has been assigned for the first time,
     *  which is used to detect when an uninitialised variable is accessed. */
    unsigned char initialised;

    /** Non-zero if this node is "red" in the name lookup red-black tree */
    unsigned char red;

    /** Non-zero if this variable is an array with a negative size */
    unsigned char negative_size;

    /** Size of the array if type is IP_TYPE_ARRAY_OF_INT or
     *  IP_TYPE_ARRAY_OF_FLOAT.  Otherwise zero. */
    ip_uint_t size;

    union {
        /** Integer value of the variable, if type is IP_TYPE_INT */
        ip_int_t ivalue;

        /** Floating-point value of the variable, if type is IP_TYPE_FLOAT */
        ip_float_t fvalue;

        /** Pointer to the array contents, if type is IP_TYPE_ARRAY_OF_INT */
        ip_int_t *iarray;

        /** Pointer to the array contents, if type is IP_TYPE_ARRAY_OF_FLOAT */
        ip_float_t *farray;
    };

    /** Left sub-tree in the name lookup red-black tree */
    ip_var_t *left;

    /** Right sub-tree in the name lookup red-black tree */
    ip_var_t *right;
};

/**
 * @brief Table of all variables in the program.
 */
typedef struct
{
    /** Root of the red-black tree */
    ip_var_t root;

    /** Sentinel node that represents a leaf */
    ip_var_t nil;

} ip_var_table_t;

/**
 * @brief Initialises a variable table.
 *
 * @param[out] vars The variable table to initialise.
 */
void ip_var_table_init(ip_var_table_t *vars);

/**
 * @brief Frees a variable table and all of the contained variables.
 *
 * @param[in] vars The variable table to free.
 */
void ip_var_table_free(ip_var_table_t *vars);

/**
 * @brief Looks up a variable in a variable table.
 *
 * @param[in] vars The variable table.
 * @param[in] name The name of the variable to look for.
 *
 * @return A pointer to the variable, or NULL if @a name was not found.
 */
ip_var_t *ip_var_lookup(const ip_var_table_t *vars, const char *name);

/**
 * @brief Creates a new variable in a variable table.
 *
 * @param[in,out] vars The variable table.
 * @param[in] name The name of the variable to create.
 * @param[in] type The type of the variable to create, which must
 * not be an array type.
 *
 * @return A pointer to the new variable, or NULL if @a name already exists.
 *
 * The new variable will be marked as uninitialised.
 */
ip_var_t *ip_var_create
    (ip_var_table_t *vars, const char *name, unsigned char type);

/**
 * @brief Dimensions an array variable.
 *
 * @param[in,out] var The variable to turn into an array.
 * @param[in] max_subscript The maximum subscript for the array, which
 * can be negative.  Indexes range from zero to @a max_subscript.
 *
 * If the variable already exists as an array, then this will redimension
 * the array to the new size while preserving as much of the original
 * contents as possible.  If the new size is greater, then the new
 * elements will be set to zero.
 */
void ip_var_dimension_array(ip_var_t *var, ip_int_t max_subscript);

#ifdef __cplusplus
}
#endif

#endif
