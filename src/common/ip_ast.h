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

#ifndef INTERPROGRAM_AST_H
#define INTERPROGRAM_AST_H

#include "ip_token.h"
#include "ip_vars.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Node in the INTERPROGRAM language's abstract syntax tree.
 */
typedef struct ip_ast_node_s ip_ast_node_t;
struct ip_ast_node_s
{
    /** Type of node; one of the ITOK_* token values */
    unsigned char type;

    /** Type of value represented by this node; e.g. IP_TYPE_INT */
    unsigned char value_type;

    /** Type of "THIS" after this node or IP_TYPE_UNKNOWN if not modified */
    unsigned char this_type;

    /** Non-zero if the node has children */
    unsigned char has_children;

    union {
        struct {

            /** Left sub-tree */
            ip_ast_node_t *left;

            /** Right sub-tree */
            ip_ast_node_t *right;

        } children; /**< Child nodes for complex abstract syntax trees */

        /** Value of an integer constant node */
        ip_int_t ivalue;

        /** Value of a floating-point constant node */
        ip_float_t fvalue;

        /** Value of a text node */
        char *text;

        /** Reference to a variable when "type" is ITOK_VAR_NAME */
        ip_var_t *var;
    };

    /** Location of the node in the original source file */
    ip_loc_t loc;

    /** Next node in a list */
    ip_ast_node_t *next;
};

/**
 * @brief List of nodes in an INTERPROGRAM source file, usually for
 * lists of statements.
 */
typedef struct
{
    /** First node in the list */
    ip_ast_node_t *first;

    /** last node in the list */
    ip_ast_node_t *last;

} ip_ast_list_t;

/**
 * @brief Frees a node in the abstract syntax tree.
 *
 * @param[in] node The node to free.
 */
void ip_ast_node_free(ip_ast_node_t *node);

/**
 * @brief Makes a new integer constant node.
 *
 * @param[in] value The constant value.
 * @param[in] loc Location of the constant in the original source file.
 *
 * @return The new node.
 */
ip_ast_node_t *ip_ast_make_int_constant(ip_int_t value, const ip_loc_t *loc);

/**
 * @brief Makes a new floating-point constant node.
 *
 * @param[in] value The constant value.
 * @param[in] loc Location of the constant in the original source file.
 *
 * @return The new node.
 */
ip_ast_node_t *ip_ast_make_float_constant(ip_float_t value, const ip_loc_t *loc);

/**
 * @brief Casts a node to a specific type.
 *
 * @param[in] type The type to cast to; one of IP_TYPE_INT, IP_TYPE_FLOAT,
 * or IP_TYPE_DYNAMIC.
 * @param[in] node The node to cast.
 *
 * @return The new node.
 *
 * May return @a node as-is if it is already of the right type.
 */
ip_ast_node_t *ip_ast_make_cast(unsigned char type, ip_ast_node_t *node);

/**
 * @brief Makes a node that represents the value of "THIS".
 *
 * @param[in] this_type The known type of "THIS" from a previous statement;
 * IP_TYPE_INT, IP_TYPE_FLOAT, or IP_TYPE_DYNAMIC.
 * @param[in] loc Location of "THIS" in the original source file.
 *
 * @return The new node.
 */
ip_ast_node_t *ip_ast_make_this(unsigned char this_type, const ip_loc_t *loc);

/**
 * @brief Makes a binary expression node.
 *
 * @param[in] type The type of the binary expression; e.g. ITOK_PLUS.
 * @param[in] left The left sub-tree for the expression.
 * @param[in] right The right sub-tree for the expression.
 * @param[in] loc Location of the expression in the original source file.
 *
 * @return The new node.
 *
 * If possible, this function will upcast the arguments to a common type.
 */
ip_ast_node_t *ip_ast_make_binary
    (unsigned char type, ip_ast_node_t *left, ip_ast_node_t *right,
     const ip_loc_t *loc);

/**
 * @brief Makes a unary expression node.
 *
 * @param[in] type The type of the unary expression; e.g. ITOK_POSITIVE.
 * @param[in] expr The sub-expression.
 * @param[in] loc Location of the expression in the original source file.
 *
 * @return The new node.
 */
ip_ast_node_t *ip_ast_make_unary
    (unsigned char type, ip_ast_node_t *expr, const ip_loc_t *loc);

/**
 * @brief Make a binary expression node where the left-hand side is "THIS".
 *
 * @param[in] type The type of the binary expression; e.g. ITOK_ADD.
 * @param[in] this_type The type of "THIS" before the expression,
 * so that it can be upcast to match the type of @a right.
 * @param[in] this_cast The type to cast "THIS" to before passing it to
 * the binary operator, or IP_TYPE_UNKNOWN to retain the current type.
 * @param[in] right The right sub-tree for the expression.
 * @param[in] loc Location of the expression in the original source file.
 *
 * @return The new node.
 *
 * It is assumed that the result will be in "THIS" after the expression.
 */
ip_ast_node_t *ip_ast_make_this_binary
    (unsigned char type, unsigned char this_type, unsigned char this_cast,
     ip_ast_node_t *right, const ip_loc_t *loc);

/**
 * @brief Make a unary expression node that operates on "THIS".
 *
 * @param[in] type The type of the unary expression; e.g. ITOK_SQRT.
 * @param[in] this_type The type of "THIS" before the expression,
 * so that it can be upcast to match @a result_type.
 * @param[in] result_type The result type for the expression.
 * @param[in] loc Location of the expression in the original source file.
 *
 * @return The new node.
 *
 * It is assumed that the result will be in "THIS" after the expression
 * and will be of type @a result_type.
 */
ip_ast_node_t *ip_ast_make_this_unary
    (unsigned char type, unsigned char this_type, unsigned char result_type,
     const ip_loc_t *loc);

/**
 * @brief Make a variable expression node.
 *
 * @param[in] var The variable that the node is referring to.
 * @param[in] loc Location of the expression in the original source file.
 *
 * @return The new node.
 */
ip_ast_node_t *ip_ast_make_variable(ip_var_t *var, const ip_loc_t *loc);

/**
 * @brief Make an array index expression node.
 *
 * @param[in] var The array variable.
 * @param[in] index Expression for the array index.
 * @param[in] loc Location of the expression in the original source file.
 *
 * @return The new node.
 */
ip_ast_node_t *ip_ast_make_array_access
    (ip_var_t *var, ip_ast_node_t *index, const ip_loc_t *loc);

/**
 * @brief Make a standalone statement node that does not have any arguments.
 *
 * @param[in] type The type of statement to make.
 * @param[in] loc Location of the statement in the original source file.
 *
 * @return The new node.
 */
ip_ast_node_t *ip_ast_make_standalone(unsigned char type, const ip_loc_t *loc);

/**
 * @brief Make a statement that takes a single argument.
 *
 * @param[in] type The type of statement to make.
 * @param[in] this_type The type of "THIS" after the statement, or
 * IP_TYPE_UNKNOWN if "THIS" is not modified.
 * @param[in] arg The node representing the argument.
 * @param[in] loc Location of the statement in the original source file.
 *
 * @return The new node.
 */
ip_ast_node_t *ip_ast_make_unary_statement
    (unsigned char type, unsigned char this_type,
     ip_ast_node_t *arg, const ip_loc_t *loc);

/**
 * @brief Make a statement that takes two arguments.
 *
 * @param[in] type The type of statement to make.
 * @param[in] this_type The type of "THIS" after the statement, or
 * IP_TYPE_UNKNOWN if "THIS" is not modified.
 * @param[in] arg1 The node representing the first argument.
 * @param[in] arg2 The node representing the second argument.
 * @param[in] loc Location of the statement in the original source file.
 *
 * @return The new node.
 */
ip_ast_node_t *ip_ast_make_binary_statement
    (unsigned char type, unsigned char this_type,
     ip_ast_node_t *arg1, ip_ast_node_t *arg2, const ip_loc_t *loc);

/**
 * @brief Make a text node.
 *
 * @param[in] type The type of node to make.
 * @param[in] text The text to put in the node.
 * @param[in] loc Location of the text in the original source file.
 *
 * @return The new node.
 */
ip_ast_node_t *ip_ast_make_text
    (unsigned char type, const char *text, const ip_loc_t *loc);

/**
 * @brief Initializes a list of nodes.
 *
 * @param[out] list The list to initialize.
 */
void ip_ast_list_init(ip_ast_list_t *list);

/**
 * @brief Frees a list of nodes.
 *
 * @param[in] list The list to free.
 */
void ip_ast_list_free(ip_ast_list_t *list);

/**
 * @brief Adds a new node to an existing list of nodes.
 *
 * @param[in,out] list The list to add to, must not be NULL.
 * @param[in] node The node to add, may be NULL.
 */
void ip_ast_list_add(ip_ast_list_t *list, ip_ast_node_t *node);

#ifdef __cplusplus
}
#endif

#endif
