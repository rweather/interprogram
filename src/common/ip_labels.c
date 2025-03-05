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

#include "ip_labels.h"
#include <stdlib.h>
#include <string.h>

void ip_label_table_init(ip_label_table_t *labels)
{
    ip_symbol_table_init(&(labels->symbols));
}

void ip_label_table_free(ip_label_table_t *labels)
{
    ip_symbol_table_free(&(labels->symbols));
}

ip_label_t *ip_label_lookup_by_name
    (const ip_label_table_t *labels, const char *name)
{
    return (ip_label_t *)ip_symbol_lookup_by_name(&(labels->symbols), name);
}

ip_label_t *ip_label_lookup_by_number
    (const ip_label_table_t *labels, ip_int_t num)
{
    return (ip_label_t *)ip_symbol_lookup_by_number(&(labels->symbols), num);
}

ip_label_t *ip_label_create_by_name(ip_label_table_t *labels, const char *name)
{
    ip_label_t *label;

    /* Bail out if the label is already in the tree */
    label = ip_label_lookup_by_name(labels, name);
    if (label) {
        return 0;
    }

    /* Construct a new label node and give it a name */
    label = calloc(1, sizeof(ip_label_t));
    if (label) {
        label->base.name = strdup(name);
        if (!(label->base.name)) {
            ip_out_of_memory();
        }
        label->base.num = -1;
    } else {
        ip_out_of_memory();
    }

    /* Insert the new label into the red-black tree */
    ip_symbol_insert(&(labels->symbols), &(label->base));
    return label;
}

ip_label_t *ip_label_create_by_number(ip_label_table_t *labels, ip_int_t num)
{
    ip_label_t *label;

    /* Bail out if the label is already in the tree */
    label = ip_label_lookup_by_number(labels, num);
    if (label) {
        return 0;
    }

    /* Construct a new label node and give it a number */
    label = calloc(1, sizeof(ip_label_t));
    if (label) {
        label->base.name = 0;
        label->base.num = num;
    } else {
        ip_out_of_memory();
    }

    /* Insert the new label into the red-black tree */
    ip_symbol_insert(&(labels->symbols), &(label->base));
    return label;
}

static void ip_label_walk_and_visit
    (ip_label_table_t *labels, ip_label_t *label,
     ip_label_visitor_t visitor, void *user_data)
{
    if (&(label->base) != &(labels->symbols.nil)) {
        ip_label_walk_and_visit
            (labels, (ip_label_t *)(label->base.left), visitor, user_data);
        (*visitor)(label, user_data);
        ip_label_walk_and_visit
            (labels, (ip_label_t *)(label->base.right), visitor, user_data);
    }
}

void ip_label_table_visit
    (ip_label_table_t *table, ip_label_visitor_t visitor, void *user_data)
{
    ip_label_walk_and_visit
        (table, (ip_label_t *)(table->symbols.root.right), visitor, user_data);
}
