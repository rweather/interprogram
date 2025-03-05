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

#include "ip_symbols.h"
#include <stdlib.h>
#include <string.h>

/* Reference for red-black trees: Algorithms in C++, Robert Sedgwick, 1992 */

void ip_symbol_table_init(ip_symbol_table_t *symbols)
{
    memset(symbols, 0, sizeof(ip_symbol_table_t));
    symbols->nil.left = &(symbols->nil);
    symbols->nil.right = &(symbols->nil);
    symbols->nil.num = -1;
    symbols->root.right = &(symbols->nil);
}

static void ip_symbol_free(ip_symbol_table_t *symbols, ip_symbol_t *symbol)
{
    if (symbol && symbol != &(symbols->nil)) {
        if (symbols->free_symbol) {
            (*(symbols->free_symbol))(symbol);
        }
        if (symbol->name) {
            free(symbol->name);
        }
        ip_symbol_free(symbols, symbol->left);
        ip_symbol_free(symbols, symbol->right);
        free(symbol);
    }
}

void ip_symbol_table_free(ip_symbol_table_t *symbols)
{
    ip_symbol_free(symbols, symbols->root.right);
    memset(symbols, 0, sizeof(ip_symbol_table_t));
}

static void ip_symbol_reset(ip_symbol_table_t *symbols, ip_symbol_t *symbol)
{
    if (symbol != &(symbols->nil)) {
        if ((symbol->flags & IP_SYMBOL_NO_RESET) == 0) {
            (*(symbols->reset_symbol))(symbol);
        }
        ip_symbol_reset(symbols, symbol->left);
        ip_symbol_reset(symbols, symbol->right);
    }
}

/* Numbers always sort less than names */

static int ip_symbol_compare_name(const char *name, const ip_symbol_t *symbol)
{
    if (!(symbol->name)) {
        return 1;
    } else {
        return strcmp(name, symbol->name);
    }
}

static int ip_symbol_compare_number(ip_int_t num, const ip_symbol_t *symbol)
{
    if (symbol->name) {
        return -1;
    } else if (num < symbol->num) {
        return -1;
    } else if (num > symbol->num) {
        return 1;
    } else {
        return 0;
    }
}

static int ip_symbol_compare(const ip_symbol_t *symbol1, const ip_symbol_t *symbol2)
{
    if (symbol1->name && symbol2->name) {
        return strcmp(symbol1->name, symbol2->name);
    } else if (symbol1->name) {
        return 1;
    } else if (symbol2->name) {
        return -1;
    } else if (symbol1->num < symbol2->num) {
        return -1;
    } else if (symbol1->num > symbol2->num) {
        return 1;
    } else {
        return 0;
    }
}

void ip_symbol_table_reset(ip_symbol_table_t *symbols)
{
    if (symbols->reset_symbol) {
        ip_symbol_reset(symbols, symbols->root.right);
    }
}

ip_symbol_t *ip_symbol_lookup_by_name
    (const ip_symbol_table_t *symbols, const char *name)
{
    ip_symbol_t *symbol = symbols->root.right;
    int cmp;
    while (symbol != &(symbols->nil)) {
        cmp = ip_symbol_compare_name(name, symbol);
        if (cmp == 0) {
            return symbol;
        } else if (cmp < 0) {
            symbol = symbol->left;
        } else {
            symbol = symbol->right;
        }
    }
    return 0;
}

ip_symbol_t *ip_symbol_lookup_by_number
    (const ip_symbol_table_t *symbols, ip_int_t num)
{
    ip_symbol_t *symbol = symbols->root.right;
    int cmp;
    while (symbol != &(symbols->nil)) {
        cmp = ip_symbol_compare_number(num, symbol);
        if (cmp == 0) {
            return symbol;
        } else if (cmp < 0) {
            symbol = symbol->left;
        } else {
            symbol = symbol->right;
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
static ip_symbol_t *ip_symbol_rotate(ip_symbol_t *node, ip_symbol_t *y)
{
    ip_symbol_t *c;
    ip_symbol_t *gc;
    int cmp, cmp2;
    cmp = ip_symbol_compare(node, y);
    if (cmp < 0) {
        c = y->left;
    } else {
        c = y->right;
    }
    cmp2 = ip_symbol_compare(node, c);
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

void ip_symbol_insert(ip_symbol_table_t *symbols, ip_symbol_t *symbol)
{
    ip_symbol_t *x = &(symbols->root);
    ip_symbol_t *p = x;
    ip_symbol_t *g = x;
    ip_symbol_t *gg = &(symbols->nil);
    int cmp, cmp2;

    /* Prepare the node for insertion */
    symbol->red = 1;
    symbol->left = &(symbols->nil);
    symbol->right = &(symbols->nil);

    /* Find the place to insert the node, rearranging the tree as needed */
    while (x != &(symbols->nil)) {
        gg = g;
        g = p;
        p = x;
        cmp = ip_symbol_compare(symbol, x);
        if (cmp < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
        if (x->left->red && x->right->red) {
            x->red = 1;
            x->left->red = 0;
            x->right->red = 0;
            if (p->red) {
                g->red = 1;
                cmp  = ip_symbol_compare(symbol, g);
                cmp2 = ip_symbol_compare(symbol, p);
                if (cmp != cmp2) {
                    p = ip_symbol_rotate(symbol, g);
                }
                x = ip_symbol_rotate(symbol, gg);
                x->red = 0;
            }
        }
    }

    /* Insert the node into the tree */
    x = symbol;
    cmp = ip_symbol_compare(x, p);
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
        cmp  = ip_symbol_compare(symbol, g);
        cmp2 = ip_symbol_compare(symbol, p);
        if (cmp != cmp2) {
            p = ip_symbol_rotate(symbol, g);
        }
        x = ip_symbol_rotate(symbol, gg);
        x->red = 0;
    }
    symbols->root.right->red = 0;
}
