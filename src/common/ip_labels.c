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

/* Reference for red-black trees: Algorithms in C++, Robert Sedgwick, 1992 */

void ip_label_table_init(ip_label_table_t *labels)
{
    memset(labels, 0, sizeof(ip_label_table_t));
    labels->nil.left = &(labels->nil);
    labels->nil.right = &(labels->nil);
    labels->nil.num = -1;
    labels->root.right = &(labels->nil);
}

static void ip_label_free(ip_label_table_t *labels, ip_label_t *label)
{
    if (label && label != &(labels->nil)) {
        if (label->name) {
            free(label->name);
        }
        ip_label_free(labels, label->left);
        ip_label_free(labels, label->right);
        free(label);
    }
}

void ip_label_table_free(ip_label_table_t *labels)
{
    ip_label_free(labels, labels->root.right);
    memset(labels, 0, sizeof(ip_label_table_t));
}

/* Numbers always sort less than names */

static int ip_label_compare_name(const char *name, const ip_label_t *label)
{
    if (!(label->name)) {
        return 1;
    } else {
        return strcmp(name, label->name);
    }
}

static int ip_label_compare_number(ip_int_t num, const ip_label_t *label)
{
    if (label->name) {
        return -1;
    } else if (num < label->num) {
        return -1;
    } else if (num > label->num) {
        return 1;
    } else {
        return 0;
    }
}

static int ip_label_compare(const ip_label_t *label1, const ip_label_t *label2)
{
    if (label1->name && label2->name) {
        return strcmp(label1->name, label2->name);
    } else if (label1->name) {
        return 1;
    } else if (label2->name) {
        return -1;
    } else if (label1->num < label2->num) {
        return -1;
    } else if (label1->num > label2->num) {
        return 1;
    } else {
        return 0;
    }
}

ip_label_t *ip_label_lookup_by_name
    (const ip_label_table_t *labels, const char *name)
{
    ip_label_t *label = labels->root.right;
    int cmp;
    while (label != &(labels->nil)) {
        cmp = ip_label_compare_name(name, label);
        if (cmp == 0) {
            return label;
        } else if (cmp < 0) {
            label = label->left;
        } else {
            label = label->right;
        }
    }
    return 0;
}

ip_label_t *ip_label_lookup_by_number
    (const ip_label_table_t *labels, ip_int_t num)
{
    ip_label_t *label = labels->root.right;
    int cmp;
    while (label != &(labels->nil)) {
        cmp = ip_label_compare_number(num, label);
        if (cmp == 0) {
            return label;
        } else if (cmp < 0) {
            label = label->left;
        } else {
            label = label->right;
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
static ip_label_t *ip_label_rotate(ip_label_t *node, ip_label_t *y)
{
    ip_label_t *c;
    ip_label_t *gc;
    int cmp, cmp2;
    cmp = ip_label_compare(node, y);
    if (cmp < 0) {
        c = y->left;
    } else {
        c = y->right;
    }
    cmp2 = ip_label_compare(node, c);
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
 * @param[in,out] labels The red-black tree to insert into.
 * @param[in] node The node to insert.  The name must not already be
 * present in the tree.
 */
static void ip_label_insert(ip_label_table_t *labels, ip_label_t *node)
{
    ip_label_t *x = &(labels->root);
    ip_label_t *p = x;
    ip_label_t *g = x;
    ip_label_t *gg = &(labels->nil);
    int cmp, cmp2;

    /* Prepare the node for insertion */
    node->red = 1;
    node->left = &(labels->nil);
    node->right = &(labels->nil);

    /* Find the place to insert the node, rearranging the tree as needed */
    while (x != &(labels->nil)) {
        gg = g;
        g = p;
        p = x;
        cmp = ip_label_compare(node, x);
        if (cmp < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
        if (!(x->left->red) && !(x->right->red)) {
            x->red = 1;
            x->left->red = 0;
            x->right->red = 0;
            if (p->red) {
                g->red = 1;
                cmp  = ip_label_compare(node, g);
                cmp2 = ip_label_compare(node, p);
                if (cmp != cmp2) {
                    p = ip_label_rotate(node, g);
                }
                x = ip_label_rotate(node, gg);
                x->red = 0;
            }
        }
    }

    /* Insert the node into the tree */
    cmp = ip_label_compare(node, x);
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
        cmp  = ip_label_compare(node, g);
        cmp2 = ip_label_compare(node, p);
        if (cmp != cmp2) {
            p = ip_label_rotate(node, g);
        }
        x = ip_label_rotate(node, gg);
        x->red = 0;
    }
    labels->root.right->red = 0;
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
        label->name = strdup(name);
        label->num = -1;
    }
    if (!label || !(label->name)) {
        ip_label_free(labels, label);
        ip_out_of_memory();
    }

    /* Insert the new label into the red-black tree */
    ip_label_insert(labels, label);
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
        label->name = 0;
        label->num = num;
    } else {
        ip_out_of_memory();
    }

    /* Insert the new label into the red-black tree */
    ip_label_insert(labels, label);
    return label;
}
