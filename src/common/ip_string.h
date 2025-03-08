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

#ifndef INTERPROGRAM_STRING_H
#define INTERPROGRAM_STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure of a reference-counted dynamic string.
 */
typedef struct
{
    /** Reference counter for this string */
    size_t ref;

    /** Length of this string, not including the NUL terminator */
    size_t len;

    /** Data for the contents of the string, NUL-terminated */
    char data[1];

} ip_string_t;

/**
 * @brief Creates a string from a NUL-terminated C string.
 *
 * @param[in] str Points to the NUL-terminated C string.
 *
 * @return The new string.
 */
ip_string_t *ip_string_create(const char *str);

/**
 * @brief Creates a string from a buffer and length.
 *
 * @param[in] str Points to the string buffer.
 * @param[in] len Length of the string.
 *
 * @return The new string.
 */
ip_string_t *ip_string_create_with_length(const char *str, size_t len);

/**
 * @brief Creates a reference to an empty string.
 *
 * @return The reference to the empty string.
 */
ip_string_t *ip_string_create_empty(void);

/**
 * @brief Gets the length of a string.
 *
 * @param[in] str The string.
 *
 * @return The length of the string.
 */
size_t ip_string_length(const ip_string_t *str);

/**
 * @brief Adds a reference to a string.
 *
 * @param[in,out] str The string to add a reference to.
 */
void ip_string_ref(ip_string_t *str);

/**
 * @brief Removes a reference from a string and deallocates it if
 * there are no more references.
 *
 * @param[in] str The string to dereference.
 */
void ip_string_deref(ip_string_t *str);

/**
 * @brief Concatenates two strings.
 *
 * @param[in] str1 The first string.
 * @param[in] str2 The second string.
 *
 * @return The concatenation of @a str1 and @a str2, which may be
 * reference to @a str1 or @a str2 if the other string is empty.
 */
ip_string_t *ip_string_concat(ip_string_t *str1, ip_string_t *str2);

/**
 * @brief Extracts a substring from a string.
 *
 * @param[in] str The string to extract from.
 * @param[in] start The starting position in the string to extract from.
 * @param[in] len The number of characters to extract.
 *
 * @return The substring, which may be a reference to @a str if @a start
 * is zero and @a len is greater than or equal to the length of @a str.
 */
ip_string_t *ip_string_substring(ip_string_t *str, size_t start, size_t len);

/**
 * @brief Determine if a character is whitespace.
 *
 * @param[in] ch The character to test.
 *
 * @return Non-zero if @a ch is whitespace, zero if not.
 */
int ip_char_is_whitespace(int ch);

/**
 * @brief Pads a string on the left with extra spaces.
 *
 * @param[in] str The string to pad.
 * @param[in] spaces The number of spaces to add on the left.
 *
 * @return The new padded string.
 */
ip_string_t *ip_string_pad_left(ip_string_t *str, size_t spaces);

/**
 * @brief Pads a string on the right with extra spaces.
 *
 * @param[in] str The string to pad.
 * @param[in] spaces The number of spaces to add on the right.
 *
 * @return The new padded string.
 */
ip_string_t *ip_string_pad_right(ip_string_t *str, size_t spaces);

/**
 * @brief Converts a string into uppercase.
 *
 * @param[in] str The string to convert.
 *
 * @return The new converted string.
 */
ip_string_t *ip_string_to_uppercase(ip_string_t *str);

/**
 * @brief Converts a string into lowercase.
 *
 * @param[in] str The string to convert.
 *
 * @return The new converted string.
 */
ip_string_t *ip_string_to_lowercase(ip_string_t *str);

#ifdef __cplusplus
}
#endif

#endif
