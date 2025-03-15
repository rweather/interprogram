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

#include "ip_ast.h"
#include <stdlib.h>
#include <string.h>

static ip_ast_node_t *ip_ast_make_node
    (unsigned char type, unsigned char value_type, const ip_loc_t *loc)
{
    ip_ast_node_t *node = (ip_ast_node_t *)calloc(1, sizeof(ip_ast_node_t));
    if (!node) {
        ip_out_of_memory();
    }
    node->type = type;
    node->value_type = value_type;
    if (loc) {
        node->loc = *loc;
    }
    return node;
}

void ip_ast_node_free(ip_ast_node_t *node)
{
    ip_ast_node_t *next;
    while (node) {
        next = node->next;
        if (node->has_children) {
            ip_ast_node_free(node->children.left);
            if (!(node->dont_free_right)) {
                /* 'THEN', 'ELSE', 'ELSE IF', 'REPEAT WHILE', and 'END REPEAT'
                 * nodes use the right child pointer as a "next" clause pointer,
                 * which will be freed separately */
                ip_ast_node_free(node->children.right);
            }
        }
        if (node->type == ITOK_EOL || node->type == ITOK_TITLE ||
                node->type == ITOK_PUNCH || node->type == ITOK_TEXT) {
            if (node->text) {
                ip_string_deref(node->text);
            }
        }
        free(node);
        node = next;
    }
}

ip_ast_node_t *ip_ast_make_int_constant(ip_int_t value, const ip_loc_t *loc)
{
    ip_ast_node_t *node = ip_ast_make_node(ITOK_INT_VALUE, IP_TYPE_INT, loc);
    node->ivalue = value;
    return node;
}

ip_ast_node_t *ip_ast_make_float_constant(ip_float_t value, const ip_loc_t *loc)
{
    ip_ast_node_t *node = ip_ast_make_node(ITOK_FLOAT_VALUE, IP_TYPE_FLOAT, loc);
    node->fvalue = value;
    return node;
}

ip_ast_node_t *ip_ast_make_cast(unsigned char type, ip_ast_node_t *node)
{
    ip_ast_node_t *node2;
    if (node && node->value_type != type) {
        if (type == IP_TYPE_INT) {
            node2 = ip_ast_make_node
                (ITOK_TO_INT, IP_TYPE_INT, &(node->loc));
            node2->has_children = 1;
            node2->children.left = node;
            node2->this_type = node->this_type;
            return node2;
        } else if (type == IP_TYPE_FLOAT) {
            node2 = ip_ast_make_node
                (ITOK_TO_FLOAT, IP_TYPE_FLOAT, &(node->loc));
            node2->has_children = 1;
            node2->children.left = node;
            node2->this_type = node->this_type;
            return node2;
        } else if (type == IP_TYPE_STRING) {
            node2 = ip_ast_make_node
                (ITOK_TO_STRING, IP_TYPE_STRING, &(node->loc));
            node2->has_children = 1;
            node2->children.left = node;
            node2->this_type = node->this_type;
        } else if (type == IP_TYPE_DYNAMIC) {
            node2 = ip_ast_make_node
                (ITOK_TO_DYNAMIC, IP_TYPE_DYNAMIC, &(node->loc));
            node2->has_children = 1;
            node2->children.left = node;
            node2->this_type = node->this_type;
            return node2;
        }
    }
    return node;
}

ip_ast_node_t *ip_ast_make_this(unsigned char this_type, const ip_loc_t *loc)
{
    ip_ast_node_t *node = ip_ast_make_node(ITOK_THIS, this_type, loc);
    node->this_type = this_type;
    return node;
}

ip_ast_node_t *ip_ast_make_binary
    (unsigned char type, ip_ast_node_t *left, ip_ast_node_t *right,
     const ip_loc_t *loc)
{
    ip_ast_node_t *node;
    unsigned char common_type;

    /* If either sub-tree is in error, free the other one and return NULL */
    if (!left || !right) {
        ip_ast_node_free(left);
        ip_ast_node_free(right);
        return 0;
    }

    /* Cast the arguments to a common type if possible */
    if (left->value_type == IP_TYPE_STRING ||
            right->value_type == IP_TYPE_STRING) {
        /* If either sub-tree is a string, then the common type is string */
        common_type = IP_TYPE_STRING;
        left = ip_ast_make_cast(IP_TYPE_STRING, left);
        right = ip_ast_make_cast(IP_TYPE_STRING, right);
    } else if (left->value_type == IP_TYPE_FLOAT) {
        /* Left sub-tree is float, so upcast right sub-tree to float */
        common_type = IP_TYPE_FLOAT;
        right = ip_ast_make_cast(IP_TYPE_FLOAT, right);
    } else if (right->value_type == IP_TYPE_FLOAT) {
        /* Right sub-tree is float, so upcast left sub-tree to float */
        common_type = IP_TYPE_FLOAT;
        left = ip_ast_make_cast(IP_TYPE_FLOAT, left);
    } else if (left->value_type == right->value_type) {
        /* Both are integer or dynamic */
        common_type = left->value_type;
    } else if (left->value_type == IP_TYPE_DYNAMIC) {
        /* Left sub-tree is dynamic, so upcast right sub-tree to dynamic */
        common_type = IP_TYPE_DYNAMIC;
        right = ip_ast_make_cast(IP_TYPE_DYNAMIC, right);
    } else {
        /* Right sub-tree is dynamic, so upcast left sub-tree to dynamic */
        common_type = IP_TYPE_DYNAMIC;
        left = ip_ast_make_cast(IP_TYPE_DYNAMIC, left);
    }

    /* Construct the binary expression node */
    node = ip_ast_make_node(type, common_type, loc);
    node->this_type = left->this_type;
    node->has_children = 1;
    node->children.left = left;
    node->children.right = right;
    return node;
}

ip_ast_node_t *ip_ast_make_binary_no_cast
    (unsigned char type, ip_ast_node_t *left, ip_ast_node_t *right,
     const ip_loc_t *loc)
{
    ip_ast_node_t *node;

    /* If either sub-tree is in error, free the other one and return NULL */
    if (!left || !right) {
        ip_ast_node_free(left);
        ip_ast_node_free(right);
        return 0;
    }

    /* Construct the binary expression node */
    node = ip_ast_make_node(type, IP_TYPE_UNKNOWN, loc);
    node->this_type = left->this_type;
    node->has_children = 1;
    node->children.left = left;
    node->children.right = right;
    return node;
}

ip_ast_node_t *ip_ast_make_unary
    (unsigned char type, ip_ast_node_t *expr, const ip_loc_t *loc)
{
    ip_ast_node_t *node;

    /* Bail out if the sub-tree is in error */
    if (!expr) {
        return 0;
    }

    /* Construct the unary expression node */
    node = ip_ast_make_node(type, expr->value_type, loc);
    node->this_type = expr->this_type;
    node->has_children = 1;
    node->children.left = expr;
    return node;
}

ip_ast_node_t *ip_ast_make_this_binary
    (unsigned char type, unsigned char this_type, unsigned char this_cast,
     ip_ast_node_t *right, const ip_loc_t *loc)
{
    ip_ast_node_t *node;

    /* Bail out if the right sub-tree is in error */
    if (!right) {
        return 0;
    }

    /* Construct a reference to "THIS" for the left sub-tree */
    node = ip_ast_make_this(this_type, loc);
    if (this_cast != IP_TYPE_UNKNOWN) {
        node = ip_ast_make_cast(this_cast, node);
    }

    /* Make a binary expression node and upcast to a common type */
    node = ip_ast_make_binary(type, node, right, loc);

    /* Set the type of "THIS" after the expression to the result type */
    node->this_type = node->value_type;
    return node;
}

ip_ast_node_t *ip_ast_make_this_unary
    (unsigned char type, unsigned char this_type, unsigned char result_type,
     const ip_loc_t *loc)
{
    ip_ast_node_t *child;
    ip_ast_node_t *node;

    /* Construct a reference to "THIS" for the child sub-tree */
    child = ip_ast_make_this(this_type, loc);

    /* Cast "THIS" the result type if necessary */
    if (child->value_type != result_type) {
        child = ip_ast_make_cast(result_type, child);
    }

    /* Construct the unary expression node */
    node = ip_ast_make_node(type, result_type, loc);
    node->has_children = 1;
    node->children.left = child;
    node->this_type = result_type;
    return node;
}

ip_ast_node_t *ip_ast_make_variable(ip_var_t *var, const ip_loc_t *loc)
{
    if (var) {
        ip_ast_node_t *node;
        node = ip_ast_make_node(ITOK_VAR_NAME, ip_var_get_type(var), loc);
        node->var = var;
        return node;
    } else {
        return 0;
    }
}

ip_ast_node_t *ip_ast_make_array_access
    (ip_var_t *var, ip_ast_node_t *index, const ip_loc_t *loc)
{
    ip_ast_node_t *var_node = ip_ast_make_variable(var, loc);
    ip_ast_node_t *node;
    unsigned char type;
    if (!var_node || !index) {
        ip_ast_node_free(var_node);
        ip_ast_node_free(index);
        return 0;
    }
    index = ip_ast_make_cast(IP_TYPE_INT, index);
    type = ip_var_get_type(var);
    if (type == IP_TYPE_ARRAY_OF_INT) {
        /* Index into an array of integers */
        node = ip_ast_make_node(ITOK_INDEX_INT, IP_TYPE_INT, loc);
        node->has_children = 1;
        node->children.left = var_node;
        node->children.right = index;
    } else if (type == IP_TYPE_ARRAY_OF_STRING ||
               type == IP_TYPE_STRING) {
        /* Index into an array of strings or into a single string */
        node = ip_ast_make_node(ITOK_INDEX_STRING, IP_TYPE_STRING, loc);
        node->has_children = 1;
        node->children.left = var_node;
        node->children.right = index;
    } else {
        /* Index into an array of floats */
        node = ip_ast_make_node(ITOK_INDEX_FLOAT, IP_TYPE_FLOAT, loc);
        node->has_children = 1;
        node->children.left = var_node;
        node->children.right = index;
    }
    node->this_type = index->this_type;
    return node;
}

ip_ast_node_t *ip_ast_make_standalone(unsigned char type, const ip_loc_t *loc)
{
    return ip_ast_make_node(type, IP_TYPE_UNKNOWN, loc);
}

ip_ast_node_t *ip_ast_make_unary_statement
    (unsigned char type, unsigned char this_type,
     ip_ast_node_t *arg, const ip_loc_t *loc)
{
    ip_ast_node_t *node;
    if (!arg) {
        /* An error occurred in the argument */
        return 0;
    }
    node = ip_ast_make_node(type, IP_TYPE_UNKNOWN, loc);
    node->this_type = this_type;
    node->has_children = 1;
    node->children.left = arg;
    return node;
}

ip_ast_node_t *ip_ast_make_binary_statement
    (unsigned char type, unsigned char this_type, 
     ip_ast_node_t *arg1, ip_ast_node_t *arg2, const ip_loc_t *loc)
{
    ip_ast_node_t *node;
    if (!arg1 || !arg2) {
        /* An error occurred in the arguments */
        ip_ast_node_free(arg1);
        ip_ast_node_free(arg2);
        return 0;
    }
    node = ip_ast_make_node(type, IP_TYPE_UNKNOWN, loc);
    node->this_type = this_type;
    node->has_children = 1;
    node->children.left = arg1;
    node->children.right = arg2;
    return node;
}

ip_ast_node_t *ip_ast_make_text
    (unsigned char type, const char *text, const ip_loc_t *loc)
{
    ip_ast_node_t *node = ip_ast_make_standalone(type, loc);
    if (text && (*text != '\0' || type != ITOK_EOL)) {
        node->text = ip_string_create(text);
    }
    return node;
}

ip_ast_node_t *ip_ast_make_argument
    (unsigned char type, ip_int_t num, ip_ast_node_t *expr,
     const ip_loc_t *loc)
{
    ip_ast_node_t *node = ip_ast_make_standalone(type, loc);
    node->has_children = 1;
    node->children.left = ip_ast_make_int_constant(num, loc);
    node->children.right = expr;
    return node;
}

ip_ast_node_t *ip_ast_make_function_invoke
    (void *handler, ip_ast_node_t *expr, const ip_loc_t *loc)
{
    ip_ast_node_t *node1;
    ip_ast_node_t *node2;
    if (!handler || !expr) {
        /* An error occurred in the arguments */
        ip_ast_node_free(expr);
        return 0;
    }
    node1 = ip_ast_make_node(ITOK_FUNCTION_NAME, IP_TYPE_UNKNOWN, loc);
    node1->builtin_handler = handler;
    node2 = ip_ast_make_node(ITOK_FUNCTION_INVOKE, IP_TYPE_DYNAMIC, loc);
    node2->has_children = 1;
    node2->children.left = node1;
    node2->children.right = expr;
    return node2;
}

void ip_ast_list_init(ip_ast_list_t *list)
{
    list->first = 0;
    list->last = 0;
}

void ip_ast_list_free(ip_ast_list_t *list)
{
    ip_ast_node_free(list->first);
    list->first = 0;
    list->last = 0;
}

void ip_ast_list_add(ip_ast_list_t *list, ip_ast_node_t *node)
{
    if (node) {
        node->next = 0;
        if (list->last) {
            list->last->next = node;
        } else {
            list->first = node;
        }
        list->last = node;
    }
}
