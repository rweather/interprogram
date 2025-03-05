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

#include "ip_symbols.h"
#include "ip_string.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Information about a variable that is stored in the variable table.
 */
typedef struct
{
    /** Base class information */
    ip_symbol_t base;

    /** Minimum array subscript if the type is an array */
    ip_int_t min_subscript;

    /** Maximum array subscript if the type is an array */
    ip_int_t max_subscript;

    union {
        /** Integer value of the variable, if type is IP_TYPE_INT */
        ip_int_t ivalue;

        /** Floating-point value of the variable, if type is IP_TYPE_FLOAT */
        ip_float_t fvalue;

        /** String value of the variable, if the type is IP_TYPE_STRING */
        ip_string_t *svalue;

        /** Pointer to the array contents, if type is IP_TYPE_ARRAY_OF_INT */
        ip_int_t *iarray;

        /** Pointer to the array contents, if type is IP_TYPE_ARRAY_OF_FLOAT */
        ip_float_t *farray;

        /** Pointer to the array contents, if type is IP_TYPE_ARRAY_OF_STRING */
        ip_string_t **sarray;
    };

} ip_var_t;

/**
 * @brief Table of all variables in the program.
 */
typedef struct
{
    ip_symbol_table_t symbols;  /**< Embedded symbol table */

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
 * @brief Resets all variables in a variable table to their default values
 * and mark them as uninitialised.
 *
 * @param[in,out] vars The variable table to reset.
 */
void ip_var_table_reset(ip_var_table_t *vars);

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
 * @param[in] min_subscript The minimum subscript for the array.
 * @param[in] max_subscript The maximum subscript for the array.
 * Must be greater than or equal to @a min_subscript.
 *
 * If the variable already exists as an array, then this will redimension
 * the array and clear the contents to zero.
 */
void ip_var_dimension_array
    (ip_var_t *var, ip_int_t min_subscript, ip_int_t max_subscript);

/**
 * @brief Determine if a variable is an array.
 *
 * @param[in] var The variable to test.
 *
 * @return Non-zero if @a var is an array, zero if not.
 */
int ip_var_is_array(const ip_var_t *var);

/**
 * @brief Gets the name of a variable.
 *
 * @param[in] var The variable.
 *
 * @return The variable's name.
 */
#define ip_var_get_name(var) ((var)->base.name)

/**
 * @brief Gets the type of a variable.
 *
 * @param[in] var The variable.
 *
 * @return The variable's type.
 */
#define ip_var_get_type(var) ((var)->base.type)

/**
 * @brief Determine if a variable has been initialised.
 *
 * @param[in] var The variable.
 *
 * @return Non-zero if the variable is initialised, zero if not.
 */
#define ip_var_is_initialised(var) \
    (((var)->base.flags & IP_SYMBOL_DEFINED) != 0)

/**
 * @brief Marks a variable as initialised.
 *
 * @param[in] var The variable.
 */
#define ip_var_mark_as_initialised(var) \
    ((var)->base.flags |= IP_SYMBOL_DEFINED)

/**
 * @brief Marks a variable as uninitialised.
 *
 * @param[in] var The variable.
 */
#define ip_var_mark_as_uninitialised(var) \
    ((var)->base.flags &= ~IP_SYMBOL_DEFINED)

#ifdef __cplusplus
}
#endif

#endif
