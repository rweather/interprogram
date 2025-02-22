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

#ifndef INTERPROGRAM_TYPES_H
#define INTERPROGRAM_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Signed integer type for INTERPROGRAM.
 */
typedef int64_t ip_int_t;

/**
 * @brief Unsigned integer type for INTERPROGRAM.
 */
typedef uint64_t ip_uint_t;

/**
 * @brief Floating-point type for INTERPROGRAM.
 */
typedef double ip_float_t;

/**
 * @brief Type is unknown; used when "THIS" is left unmodified.
 */
#define IP_TYPE_UNKNOWN         0

/**
 * @brief Type of the value is determined dynamically at runtime.
 */
#define IP_TYPE_DYNAMIC         1

/**
 * @brief Type of the value is known to be integer.
 */
#define IP_TYPE_INT             2

/**
 * @brief Type of the value is known to be floating-point.
 */
#define IP_TYPE_FLOAT           3

/**
 * @brief Type of the value is known to be a string.
 */
#define IP_TYPE_STRING          4

/**
 * @brief Type of a variable is an array of integers.
 */
#define IP_TYPE_ARRAY_OF_INT    5

/**
 * @brief Type of a variable is an array of floating-point values.
 */
#define IP_TYPE_ARRAY_OF_FLOAT  6

/**
 * @brief Type of a variable is an array of string values.
 */
#define IP_TYPE_ARRAY_OF_STRING 7

/**
 * @brief Report out of memory and exit the program.
 */
void ip_out_of_memory(void);

#ifdef __cplusplus
}
#endif

#endif
