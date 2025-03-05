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

#ifndef INTERPROGRAM_SYMBOLS_H
#define INTERPROGRAM_SYMBOLS_H

#include "ip_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common symbol flags */
#define IP_SYMBOL_DEFINED   0x0001  /**< Symbol has been defined/initialised */
#define IP_SYMBOL_NO_RESET  0x0002  /**< Do not reset the symbol */

/**
 * @brief Information about a symbol name in a symbol table.
 */
typedef struct ip_symbol_s ip_symbol_t;
struct ip_symbol_s
{
    /** Name of the symbol if it is alphabetic, or NULL if numeric */
    char *name;

    /** Number for the symbol if it is numeric, or -1 if alphabetic */
    ip_int_t num;

    /** Non-zero if this node is "red" in the name lookup red-black tree */
    unsigned char red;

    /** Type of symbol */
    unsigned char type;

    /** Extra flags for the symbol */
    unsigned short flags;

    /** Left sub-tree in the name lookup red-black tree */
    ip_symbol_t *left;

    /** Right sub-tree in the name lookup red-black tree */
    ip_symbol_t *right;

    /* Subclasses of this structure add extra fields here */
};

/**
 * @brief Table of named symbols.
 */
typedef struct
{
    /** Root of the red-black tree */
    ip_symbol_t root;

    /** Sentinel node that represents a leaf */
    ip_symbol_t nil;

    /** Function to free a symbol's extra fields prior to free'ing the symbol */
    void (*free_symbol)(ip_symbol_t *symbol);

    /** Function to reset the extra fields of a symbol */
    void (*reset_symbol)(ip_symbol_t *symbol);

} ip_symbol_table_t;

/**
 * @brief Initialises a symbol table.
 *
 * @param[out] symbols The symbol table to initialise.
 */
void ip_symbol_table_init(ip_symbol_table_t *symbols);

/**
 * @brief Frees a symbol table and all of the contained symbols.
 *
 * @param[in] symbols The symbol table to free.
 */
void ip_symbol_table_free(ip_symbol_table_t *symbols);

/**
 * @brief Resets all symbols in a symbol table to their default values
 * and mark them as uninitialised.
 *
 * @param[in,out] symbols The symbol table to reset.
 */
void ip_symbol_table_reset(ip_symbol_table_t *symbols);

/**
 * @brief Looks up a symbol in a symbol table by name.
 *
 * @param[in] symbols The symbol table.
 * @param[in] name The name of the symbol to look for.
 *
 * @return A pointer to the symbol, or NULL if @a name was not found.
 */
ip_symbol_t *ip_symbol_lookup_by_name
    (const ip_symbol_table_t *symbols, const char *name);

/**
 * @brief Looks up a symbol in a symbol table by number.
 *
 * @param[in] symbols The symbol table.
 * @param[in] num The number of the symbol to look for.
 *
 * @return A pointer to the symbol, or NULL if @a name was not found.
 */
ip_symbol_t *ip_symbol_lookup_by_number
    (const ip_symbol_table_t *symbols, ip_int_t num);

/**
 * @brief Inserts a symbol into a symbol table.
 *
 * @param[in,out] symbols The symbol table.
 * @param[in] symbol The symbol to insert.
 *
 * It is assumed that all fields have been initialised except for
 * "left", "right", and "red"; and that the symbol name does not
 * already exist in the symbol table.
 */
void ip_symbol_insert(ip_symbol_table_t *symbols, ip_symbol_t *symbol);

#ifdef __cplusplus
}
#endif

#endif
