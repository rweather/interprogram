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

#ifndef INTERPROGRAM_VALUE_H
#define INTERPROGRAM_VALUE_H

#include "ip_types.h"
#include "ip_vars.h"
#include "ip_string.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Value that has been evaluated by the program.
 */
typedef struct
{
    /** Type of value; e.g. IP_TYPE_INT */
    unsigned char type;

    union {
        /** Integer value */
        ip_int_t ivalue;

        /** Floating-point value */
        ip_float_t fvalue;

        /** String value (reference counted) */
        ip_string_t *svalue;
    };

} ip_value_t;

/**
 * @brief Initialises a value to unknown.
 *
 * @param[out] value The value to initialise.
 */
void ip_value_init(ip_value_t *value);

/**
 * @brief Releases the memory associated with a value just before
 * it goes out of scope.
 *
 * @param[in,out] value The value to release.
 */
void ip_value_release(ip_value_t *value);

/**
 * @brief Assigns one value to another.
 *
 * @param[in,out] dest Destination to assign to.
 * @param[in] src Source value to assign.
 */
void ip_value_assign(ip_value_t *dest, const ip_value_t *src);

/**
 * @brief Sets a value to an integer.
 *
 * @param[in,out] dest Destination to assign to.
 * @param[in] src Source value to assign.
 */
void ip_value_set_int(ip_value_t *dest, ip_int_t src);

/**
 * @brief Sets a value to a floating-point number.
 *
 * @param[in,out] dest Destination to assign to.
 * @param[in] src Source value to assign.
 */
void ip_value_set_float(ip_value_t *dest, ip_float_t src);

/**
 * @brief Sets a value to a string.
 *
 * @param[in,out] dest Destination to assign to.
 * @param[in] src Source string to assign.  The reference count on
 * the string will be incremented.
 */
void ip_value_set_string(ip_value_t *dest, ip_string_t *str);

/**
 * @brief Sets a value to unknown.
 *
 * @param[in,out] value Value to be set to unknown.
 */
void ip_value_set_unknown(ip_value_t *value);

/**
 * @brief Converts a value into an integer.
 *
 * @param[in,out] value The value to be converted.
 *
 * @return IP_EXEC_OK or an error code if the value cannot be converted.
 */
int ip_value_to_int(ip_value_t *value);

/**
 * @brief Converts a value into a floating-point.
 *
 * @param[in,out] value The value to be converted.
 *
 * @return IP_EXEC_OK or an error code if the value cannot be converted.
 */
int ip_value_to_float(ip_value_t *value);

/**
 * @brief Converts a value into a string.
 *
 * @param[in,out] value The value to be converted.
 *
 * @return IP_EXEC_OK or an error code if the value cannot be converted.
 */
int ip_value_to_string(ip_value_t *value);

/**
 * @brief Assigns the contents of a variable to a value.
 *
 * @param[in,out] dest Destination to assign to.
 * @param[in] src The source variable to assign.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_value_from_var(ip_value_t *dest, const ip_var_t *src);

/**
 * @brief Assigns a value to a destination variable.
 *
 * @param[in,out] dest Destination variable to assign to.
 * @param[in] src The source value to assign.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_value_to_var(ip_var_t *dest, const ip_value_t *src);

/**
 * @brief Copies an array element into a value.
 *
 * @param[in,out] dest Destination value to assign to.
 * @param[in] src The source array variable.
 * @param[in] index The index within the array to access.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_value_from_array(ip_value_t *dest, const ip_var_t *src, ip_int_t index);

/**
 * @brief Copies a value into an array element.
 *
 * @param[in,out] dest Destination array variable.
 * @param[in] index The index within the array to access.
 * @param[in] src The source value to assign.
 *
 * @return IP_EXEC_OK or an error code.
 */
int ip_value_to_array(ip_var_t *dest, ip_int_t index, const ip_value_t *src);

#ifdef __cplusplus
}
#endif

#endif
