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

#include "ip_program.h"
#include <inttypes.h>

static void ip_program_list_integer_symbols
    (const ip_program_t *program, const ip_var_t *var,
     int *saw_arrays, int *saw_integers, FILE *file)
{
    if (var != &(program->vars.nil)) {
        ip_program_list_integer_symbols
            (program, var->left, saw_arrays, saw_integers, file);
        if (var->type == IP_TYPE_INT || var->type == IP_TYPE_ARRAY_OF_INT) {
            if (*saw_integers) {
                fprintf(file, ", %s", var->name);
            } else {
                fprintf(file, "%s", var->name);
                *saw_integers = 1;
            }
        }
        if (var->type == IP_TYPE_ARRAY_OF_INT ||
                var->type == IP_TYPE_ARRAY_OF_FLOAT ||
                var->type == IP_TYPE_ARRAY_OF_STRING) {
            *saw_arrays = 1;
        }
        ip_program_list_integer_symbols
            (program, var->right, saw_arrays, saw_integers, file);
    }
}

static void ip_program_list_string_symbols
    (const ip_program_t *program, const ip_var_t *var,
     int *saw_strings, FILE *file)
{
    if (var != &(program->vars.nil)) {
        ip_program_list_string_symbols(program, var->left, saw_strings, file);
        if (var->type == IP_TYPE_STRING ||
                var->type == IP_TYPE_ARRAY_OF_STRING) {
            if (*saw_strings) {
                fprintf(file, ", %s", var->name);
            } else {
                fprintf(file, "(2) SYMBOLS FOR STRINGS %s", var->name);
                *saw_strings = 1;
            }
        }
        ip_program_list_string_symbols(program, var->right, saw_strings, file);
    }
}

static int ip_program_list_symbols(const ip_program_t *program, FILE *file)
{
    int saw_arrays = 0;
    int saw_integers = 0;
    int saw_strings = 0;
    fprintf(file, "(2) SYMBOLS FOR INTEGERS ");
    ip_program_list_integer_symbols
        (program, program->vars.root.right, &saw_arrays, &saw_integers, file);
    if (!saw_integers) {
        fprintf(file, "NONE\n");
    } else {
        fprintf(file, "\n");
    }
    ip_program_list_string_symbols
        (program, program->vars.root.right, &saw_strings, file);
    if (saw_strings) {
        fprintf(file, "\n");
    }
    return saw_arrays;
}

static void ip_program_list_array_symbols
    (const ip_program_t *program, const ip_var_t *var,
     int *saw_arrays, FILE *file)
{
    if (var != &(program->vars.nil)) {
        ip_program_list_array_symbols
            (program, var->left, saw_arrays, file);
        if (var->type == IP_TYPE_ARRAY_OF_INT ||
                var->type == IP_TYPE_ARRAY_OF_FLOAT ||
                var->type == IP_TYPE_ARRAY_OF_STRING) {
            if (*saw_arrays) {
                fprintf(file, ", %s", var->name);
            } else {
                fprintf(file, "%s", var->name);
                *saw_arrays = 1;
            }
            if (var->min_subscript == 0) {
                fprintf(file, "(%ld)", (long)(var->max_subscript));
            } else if (var->max_subscript == 0) {
                fprintf(file, "(%ld)", (long)(var->min_subscript));
            } else {
                fprintf(file, "(%ld:%ld)",
                        (long)(var->min_subscript),
                        (long)(var->max_subscript));
            }
        }
        ip_program_list_array_symbols
            (program, var->right, saw_arrays, file);
    }
}

static void ip_program_list_arrays(const ip_program_t *program, FILE *file)
{
    int saw_arrays = 0;
    fprintf(file, "(3) MAXIMUM SUBSCRIPTS ");
    ip_program_list_array_symbols
        (program, program->vars.root.right, &saw_arrays, file);
    fprintf(file, "\n");
}

static void ip_program_list_node_name
    (const ip_ast_node_t *node, FILE *file)
{
    const ip_token_info_t *info = ip_tokeniser_get_keyword(node->type);
    fprintf(file, "%s", info->name);
}

static void ip_program_list_expression
    (const ip_ast_node_t *expr, int parent_priority, FILE *file)
{
    int priority = parent_priority;

    /* Get the priority of the current operator to determine
     * if we need to surround the sub-expression with parentheses */
    switch (expr->type) {
    case ITOK_THIS:
    case ITOK_VAR_NAME:
    case ITOK_INT_VALUE:
    case ITOK_FLOAT_VALUE:
    case ITOK_INDEX_INT:
    case ITOK_INDEX_FLOAT:
    case ITOK_LENGTH_OF:
        priority = 10;
        break;

    case ITOK_TO_INT:
    case ITOK_TO_FLOAT:
    case ITOK_TO_STRING:
    case ITOK_TO_DYNAMIC:
        /* Coercion node that is invisible in the output */
        ip_program_list_expression(expr->children.left, parent_priority, file);
        return;

    case ITOK_PLUS:
    case ITOK_MINUS:
        priority = 4;
        break;

    case ITOK_MUL:
    case ITOK_DIV:
        priority = 5;
        break;

    case ITOK_GREATER_THAN:
    case ITOK_MUCH_GREATER_THAN:
    case ITOK_SMALLER_THAN:
    case ITOK_MUCH_SMALLER_THAN:
    case ITOK_ZERO:
    case ITOK_POSITIVE:
    case ITOK_NEGATIVE:
    case ITOK_NOT_EQUAL_TO:
    case ITOK_GREATER_OR_EQUAL:
    case ITOK_SMALLER_OR_EQUAL:
    case ITOK_NOT_ZERO:
    case ITOK_FINITE:
    case ITOK_INFINITE:
    case ITOK_NAN:
    case ITOK_EMPTY:
    case ITOK_NOT_EMPTY:
        priority = 0;
        break;

    default:
        fprintf(file, "<UNKNOWN-EXPRESSION-%02X>", expr->type);
        return;
    }

    /* Print a left parenthesis if necessary */
    if (priority < parent_priority) {
        fputc('(', file);
    }

    /* Print the expression */
    switch (expr->type) {
    case ITOK_THIS:
        fprintf(file, "THIS");
        break;

    case ITOK_VAR_NAME:
        fprintf(file, "%s", expr->var->name);
        break;

    case ITOK_INT_VALUE:
        fprintf(file, "%" PRId64, (int64_t)(expr->ivalue));
        break;

    case ITOK_FLOAT_VALUE:
        fprintf(file, "%f", (double)(expr->fvalue));
        break;

    case ITOK_INDEX_INT:
    case ITOK_INDEX_FLOAT:
    case ITOK_INDEX_STRING:
        /* Index into an array variable */
        fprintf(file, "%s(", expr->children.left->var->name);
        ip_program_list_expression(expr->children.right, 0, file);
        fprintf(file, ")");
        break;

    case ITOK_PLUS:
    case ITOK_MINUS:
    case ITOK_MUL:
    case ITOK_DIV:
    case ITOK_GREATER_THAN:
    case ITOK_MUCH_GREATER_THAN:
    case ITOK_SMALLER_THAN:
    case ITOK_MUCH_SMALLER_THAN:
    case ITOK_NOT_EQUAL_TO:
    case ITOK_GREATER_OR_EQUAL:
    case ITOK_SMALLER_OR_EQUAL:
        /* Binary infix operator */
        ip_program_list_expression(expr->children.left, priority, file);
        fputc(' ', file);
        ip_program_list_node_name(expr, file);
        fputc(' ', file);
        ip_program_list_expression(expr->children.right, priority, file);
        break;

    case ITOK_ZERO:
    case ITOK_POSITIVE:
    case ITOK_NEGATIVE:
    case ITOK_NOT_ZERO:
    case ITOK_FINITE:
    case ITOK_INFINITE:
    case ITOK_NAN:
    case ITOK_EMPTY:
    case ITOK_NOT_EMPTY:
        /* Unary postfix operator */
        ip_program_list_expression(expr->children.left, priority, file);
        fputc(' ', file);
        ip_program_list_node_name(expr, file);
        break;

    case ITOK_LENGTH_OF:
        ip_program_list_node_name(expr, file);
        fputc(' ', file);
        ip_program_list_expression(expr->children.left, priority, file);
        break;
    }

    /* Print a right parenthesis if necessary */
    if (priority < parent_priority) {
        fputc(')', file);
    }
}

static void ip_program_list_unary_statement
    (const ip_program_t *program, const char *name,
     const ip_ast_node_t *expr, FILE *file)
{
    (void)program;
    if (expr) {
        fprintf(file, "%s ", name);
        ip_program_list_expression(expr, 0, file);
    } else {
        fprintf(file, "%s", name);
    }
}

static void ip_program_list_label
    (const ip_ast_node_t *node, FILE *file, int must_have_star)
{
    if (node->type != ITOK_LABEL || !(node->label->name) || must_have_star) {
        fputc('*', file);
    }
    if (node->type == ITOK_LABEL) {
        if (node->label->name) {
            fprintf(file, "%s", node->label->name);
        } else {
            fprintf(file, "%u", (unsigned)(node->label->num));
        }
    } else {
        ip_program_list_expression(node, 0, file);
    }
}

static void ip_program_list_node
    (const ip_program_t *program, const ip_ast_node_t *node,
     int *last_was_eol, int *added_comma, FILE *file)
{
    *added_comma = 0;
    *last_was_eol = 0;

    switch (node->type) {
    case ITOK_LABEL:
        ip_program_list_label(node, file, 1);
        fputc(' ', file);
        *last_was_eol = 1;
        break;

    case ITOK_TITLE:
        fprintf(file, "(1) TITLE %s\n", node->text ? node->text->data : "");
        if (ip_program_list_symbols(program, file)) {
            ip_program_list_arrays(program, file);
        }
        *last_was_eol = 1;
        break;

    case ITOK_COMPILE_PROGRAM:
        fprintf(file, "(4) COMPILE THE FOLLOWING INTERPROGRAM\n");
        *last_was_eol = 1;
        break;

    case ITOK_END_PROGRAM:
        fprintf(file, "END OF INTERPROGRAM");
        break;

    case ITOK_EXIT_PROGRAM:
        fprintf(file, "EXIT INTERPROGRAM");
        break;

    case ITOK_END_PROCESS:
        fprintf(file, "END OF PROCESS DEFINITION");
        break;

    case ITOK_TAKE:
        ip_program_list_unary_statement
            (program, "TAKE", node->children.left, file);
        break;

    case ITOK_ADD:
        ip_program_list_unary_statement
            (program, "ADD", node->children.right, file);
        break;

    case ITOK_SUBTRACT:
        ip_program_list_unary_statement
            (program, "SUBTRACT", node->children.right, file);
        break;

    case ITOK_MULTIPLY:
        ip_program_list_unary_statement
            (program, "MULTIPLY BY", node->children.right, file);
        break;

    case ITOK_DIVIDE:
        ip_program_list_unary_statement
            (program, "DIVIDE BY", node->children.right, file);
        break;

    case ITOK_IF:
        ip_program_list_unary_statement
            (program, "IF", node->children.left, file);
        break;

    case ITOK_REPLACE:
        ip_program_list_unary_statement
            (program, "REPLACE", node->children.left, file);
        break;

    case ITOK_INPUT:
        ip_program_list_unary_statement
            (program, "INPUT", node->children.left, file);
        break;

    case ITOK_OUTPUT:
    case ITOK_OUTPUT_NO_EOL:
        ip_program_list_unary_statement
            (program, "OUTPUT", node->children.left, file);
        if (node->type == ITOK_OUTPUT_NO_EOL) {
            *added_comma = 1;
            fputc(',', file);
        }
        break;

    case ITOK_SET:
        fprintf(file, "SET ");
        ip_program_list_expression(node->children.left, 0, file);
        fprintf(file, " = ");
        ip_program_list_expression(node->children.right, 0, file);
        break;

    case ITOK_GO_TO:
    case ITOK_EXECUTE_PROCESS:
    case ITOK_CALL:
        ip_program_list_node_name(node, file);
        fputc(' ', file);
        ip_program_list_label(node->children.left, file, 0);
        break;

    case ITOK_REPEAT_FROM:
        fprintf(file, "REPEAT FROM ");
        ip_program_list_label(node->children.left, file, 0);
        fputc(' ', file);
        ip_program_list_expression(node->children.right, 0, file);
        fprintf(file, " TIMES");
        break;

    case ITOK_PUNCH:
        fprintf(file, "PUNCH THE FOLLOWING CHARACTERS\n");
        fprintf(file, "%s~~~~~", node->text ? node->text->data : "");
        break;

    case ITOK_PUNCH_NO_BLANKS:
        fprintf(file, "PUNCH THE FOLLOWING CHARACTERS,\n");
        fprintf(file, "%s~~~~~", node->text ? node->text->data : "");
        break;

    case ITOK_PAUSE:
    case ITOK_COPY_TAPE:
    case ITOK_IGNORE_TAPE:
    case ITOK_SQRT:
    case ITOK_SIN:
    case ITOK_COS:
    case ITOK_TAN:
    case ITOK_ATAN:
    case ITOK_SIN_RADIANS:
    case ITOK_COS_RADIANS:
    case ITOK_TAN_RADIANS:
    case ITOK_ATAN_RADIANS:
    case ITOK_SIN_DEGREES:
    case ITOK_COS_DEGREES:
    case ITOK_TAN_DEGREES:
    case ITOK_ATAN_DEGREES:
    case ITOK_LOG:
    case ITOK_EXP:
    case ITOK_ABS:
    case ITOK_LENGTH_OF:
        ip_program_list_node_name(node, file);
        break;

    case ITOK_COPY_NO_BLANKS:
        fprintf(file, "COPY TAPE,");
        break;

    case ITOK_MODULO:
        ip_program_list_unary_statement
            (program, "MODULO", node->children.right, file);
        break;

    case ITOK_BITWISE_AND_NOT:
        ip_program_list_unary_statement
            (program, "BITWISE AND WITH NOT", node->children.right, file);
        break;

    case ITOK_BITWISE_AND:
        ip_program_list_unary_statement
            (program, "BITWISE AND WITH", node->children.right, file);
        break;

    case ITOK_BITWISE_OR:
        ip_program_list_unary_statement
            (program, "BITWISE OR WITH", node->children.right, file);
        break;

    case ITOK_BITWISE_XOR:
        ip_program_list_unary_statement
            (program, "BITWISE XOR WITH", node->children.right, file);
        break;

    case ITOK_BITWISE_NOT:
        fprintf(file, "BITWISE NOT");
        break;

    case ITOK_SHIFT_LEFT:
        ip_program_list_unary_statement
            (program, "SHIFT LEFT BY", node->children.right, file);
        break;

    case ITOK_SHIFT_RIGHT:
        ip_program_list_unary_statement
            (program, "SHIFT RIGHT BY", node->children.right, file);
        break;

    case ITOK_RAISE:
        ip_program_list_unary_statement
            (program, "RAISE TO POWER OF", node->children.right, file);
        break;

    case ITOK_RETURN:
        if (node->children.left) {
            ip_program_list_unary_statement
                (program, "RETURN", node->children.left, file);
        } else {
            fprintf(file, "RETURN");
        }
        break;

    case ITOK_SUBSTRING:
        fprintf(file, "SUBSTRING FROM ");
        ip_program_list_expression(node->children.left, 0, file);
        if (node->children.right) {
            fprintf(file, " TO ");
            ip_program_list_expression(node->children.right, 0, file);
        }
        break;

    case ITOK_EOL:
        if (node->text) {
            /* This line ends in a comment */
            fprintf(file, "#%s\n", node->text->data);
        } else {
            fputc('\n', file);
        }
        *last_was_eol = 1;
        break;

    default:
        fprintf(file, "<UNKNOWN-TOKEN-%02X>", node->type);
        break;
    }
}

void ip_program_list(const ip_program_t *program, FILE *file)
{
    const ip_ast_node_t *node = program->statements.first;
    int last_was_eol = 1;
    int added_comma = 0;
    while (node) {
        if (!last_was_eol) {
            if (!added_comma && node->type != ITOK_EOL) {
                fputc(',', file);
            }
            fputc(' ', file);
        } else {
            last_was_eol = 1;
        }
        ip_program_list_node(program, node, &last_was_eol, &added_comma, file);
        node = node->next;
    }
}
