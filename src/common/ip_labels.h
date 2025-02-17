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
    /** Name of the label if it is alphabetic, or NULL if numeric */
    char *name;

    /** Number for the label if it is numeric, or -1 if alphabetic */
    ip_int_t num;

    /** Non-zero if this node is "red" in the label lookup red-black tree */
    unsigned char red;

    /** Non-zero if this label is defined */
    unsigned char is_defined;

    /** Points to the node in the program corresponding to the label */
    ip_ast_node_t *node;

    /** Left sub-tree in the label lookup red-black tree */
    ip_label_t *left;

    /** Right sub-tree in the label lookup red-black tree */
    ip_label_t *right;
};

/**
 * @brief Table of all labels in the program.
 */
typedef struct
{
    /** Root of the red-black tree */
    ip_label_t root;

    /** Sentinel node that represents a leaf */
    ip_label_t nil;

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

#ifdef __cplusplus
}
#endif

#endif
