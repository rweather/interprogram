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

#ifndef INTERPROGRAM_LABELS_H
#define INTERPROGRAM_LABELS_H

#include "ip_ast.h"
#include "ip_symbols.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Minimum number that can be used for a label */
#define IP_MIN_LABEL_NUMBER 1

/** Maximum number that can be used for a label */
#define IP_MAX_LABEL_NUMBER 9999 /* Classic INTERPROGRAM used 150 */

/**
 * @brief Information about a label that is stored in the global
 * label lookup red-black tree.
 */
struct ip_label_s
{
    /** Base class information */
    ip_symbol_t base;

    /** Points to the node in the program corresponding to the label */
    ip_ast_node_t *node;
};

/**
 * @brief Table of all labels in the program.
 */
typedef struct
{
    ip_symbol_table_t symbols;  /**< Embedded symbol table */

} ip_label_table_t;

/**
 * @brief Initialises a label table.
 *
 * @param[out] labels The label table to initialise.
 */
void ip_label_table_init(ip_label_table_t *labels);

/**
 * @brief Frees a label table and all of the contained labels.
 *
 * @param[in] labels The label table to free.
 */
void ip_label_table_free(ip_label_table_t *labels);

/**
 * @brief Looks up a label in a label table by name.
 *
 * @param[in] labels The label table.
 * @param[in] name The name of the label to look for.
 *
 * @return A pointer to the label, or NULL if @a name was not found.
 */
ip_label_t *ip_label_lookup_by_name
    (const ip_label_table_t *labels, const char *name);

/**
 * @brief Looks up a label in a label table by number.
 *
 * @param[in] labels The label table.
 * @param[in] num The number of the label to look for.
 *
 * @return A pointer to the label, or NULL if @a num was not found.
 */
ip_label_t *ip_label_lookup_by_number
    (const ip_label_table_t *labels, ip_int_t num);

/**
 * @brief Creates a new label in a label table by name.
 *
 * @param[in,out] labels The label table.
 * @param[in] name The name of the label to create.
 *
 * @return A pointer to the new label, or NULL if @a name already exists.
 *
 * The new label will be marked as undefined.
 */
ip_label_t *ip_label_create_by_name
    (ip_label_table_t *labels, const char *name);

/**
 * @brief Creates a new label in a label table by number.
 *
 * @param[in,out] labels The label table.
 * @param[in] num The number of the label to create.
 *
 * @return A pointer to the new label, or NULL if @a num already exists.
 *
 * The new label will be marked as undefined.
 */
ip_label_t *ip_label_create_by_number
    (ip_label_table_t *labels, ip_int_t num);

/**
 * @brief Gets the name of a label.
 *
 * @param[in] label The label.
 *
 * @return A pointer to the name of the label or NULL if the label is numeric.
 */
#define ip_label_get_name(labell) ((label)->base.name)

/**
 * @brief Gets the number of a label.
 *
 * @param[in] label The label.
 *
 * @return The number of the label or -1 if the label is alphabetic.
 */
#define ip_label_get_number(labell) ((label)->base.num)

/**
 * @brief Determine if a label is defined.
 *
 * @param[in] label The label.
 *
 * @return Non-zero if the label is defined, zero if not.
 */
#define ip_label_is_defined(label) \
    (((label)->base.flags & IP_SYMBOL_DEFINED) != 0)

/**
 * @brief Marks a label as defined.
 *
 * @param[in] label The label.
 */
#define ip_label_mark_as_defined(label) \
    ((label)->base.flags |= IP_SYMBOL_DEFINED)

/**
 * @brief Callback function for ip_label_table_visit().
 *
 * @param[in] label The label that is being visited.
 * @param[in] user_data Context for the visit operation.
 */
typedef void (*ip_label_visitor_t)(ip_label_t *label, void *user_data);

/**
 * @brief Visits all labels in a table in order.
 *
 * @param[in] labels The label table.
 * @param[in] visitor The function to call for each label.
 * @param[in] user_data Context for the visit operation.
 */
void ip_label_table_visit
    (ip_label_table_t *labels, ip_label_visitor_t visitor, void *user_data);

#ifdef __cplusplus
}
#endif

#endif
