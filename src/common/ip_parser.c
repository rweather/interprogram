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

#include "ip_parser.h"
#include "ip_value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Forward declarations */
static ip_ast_node_t *ip_parse_extended_expression(ip_parser_t *parser);

void ip_parse_init(ip_parser_t *parser)
{
    memset(parser, 0, sizeof(ip_parser_t));
    ip_tokeniser_init(&(parser->tokeniser));
    parser->this_type = IP_TYPE_DYNAMIC;
    parser->last_statement = -1;
}

static void ip_parse_create_block
    (ip_parser_t *parser, int type, ip_ast_node_t *node)
{
    ip_parse_block_context_t *block;
    block = calloc(1, sizeof(ip_parse_block_context_t));
    if (!block) {
        ip_out_of_memory();
    }
    block->type = type;
    block->control = node;
    block->patch = node;
    block->next = parser->blocks;
    parser->blocks = block;
}

static void ip_parse_free_top_block(ip_parser_t *parser)
{
    ip_parse_block_context_t *block = parser->blocks;
    parser->blocks = block->next;
    free(block);
}

void ip_parse_free(ip_parser_t *parser)
{
    ip_tokeniser_free(&(parser->tokeniser));
    while (parser->blocks) {
        ip_parse_free_top_block(parser);
    }
    memset(parser, 0, sizeof(ip_parser_t));
}

/**
 * @brief Get the next token from the input stream.
 *
 * @param[in,out] parser The parser state.
 * @param[in] context Token context to use to recognise the next token.
 */
static void ip_parse_get_next(ip_parser_t *parser, unsigned context)
{
    ip_tokeniser_get_next(&(parser->tokeniser), parser->flags | context);
}

/**
 * @brief Negates a node in the abstract syntax tree.
 *
 * @param[in] parser The parser state.
 * @param[in] node The node to be negated.
 *
 * @return The new node.
 */
static ip_ast_node_t *ip_parse_negate_node
    (ip_parser_t *parser, ip_ast_node_t *node)
{
    if (node->value_type == IP_TYPE_FLOAT) {
        node = ip_ast_make_binary
            (ITOK_MINUS, ip_ast_make_float_constant(0, &(node->loc)), node,
             &(parser->tokeniser.loc));
    } else {
        node = ip_ast_make_binary
            (ITOK_MINUS, ip_ast_make_int_constant(0, &(node->loc)), node,
             &(parser->tokeniser.loc));
    }
    return node;
}

/** Set if array expressions are allowed in a variable expression */
#define IP_VAR_ALLOW_ARRAYS 0x0001

/** Set if parsing an r-value instead of an l-value */
#define IP_VAR_ALLOW_RVALUES 0x0002

/** Set if local variables @n references are allowed */
#define IP_VAR_ALLOW_LOCALS 0x0004

/*
 * Variable ::= VAR-NAME [ '(' Expression ')' ]
 */
static ip_ast_node_t *ip_parse_variable_expression
    (ip_parser_t *parser, int allowed)
{
    ip_ast_node_t *node = 0;
    ip_var_t *var;

    /* Check for local variable references */
    if (parser->tokeniser.token == ITOK_ARG_NUMBER &&
            (allowed & IP_VAR_ALLOW_LOCALS) != 0) {
        node = ip_ast_make_int_constant
            (parser->tokeniser.ivalue, &(parser->tokeniser.loc));
        node->type = ITOK_ARG_NUMBER;
        node->value_type = IP_TYPE_DYNAMIC;
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        return node;
    }

    /* Must have a variable name token at this position */
    if (parser->tokeniser.token != ITOK_VAR_NAME) {
        ip_error(parser, "variable name expected");
        return 0;
    }

    /* Look up the variable name */
    var = ip_var_lookup
        (&(parser->program->vars), parser->tokeniser.token_info->name);
    if (!var) {
        /* Variable does not exist, so create a new one of type float */
        var = ip_var_create
            (&(parser->program->vars), parser->tokeniser.token_info->name,
             IP_TYPE_FLOAT);
    }

    /* Advance to the next token */
    ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);

    /* Create the variable node */
    switch (ip_var_get_type(var)) {
    case IP_TYPE_INT:
    case IP_TYPE_FLOAT:
        node = ip_ast_make_variable(var, &(parser->tokeniser.loc));
        if ((allowed & IP_VAR_ALLOW_ARRAYS) != 0 &&
                parser->tokeniser.token == ITOK_LPAREN) {
            /* We cannot use an index with this variable, but parse it anyway */
            ip_error(parser, "variable '%s' is not an array",
                     ip_var_get_name(var));
            ip_ast_node_free(ip_parse_expression(parser));
        }
        break;

    case IP_TYPE_STRING:
        if ((allowed & IP_VAR_ALLOW_RVALUES) != 0 &&
                (allowed & IP_VAR_ALLOW_ARRAYS) != 0 &&
                parser->tokeniser.token == ITOK_LPAREN) {
            /* Indexing into a string to extract a character */
            ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
            node = ip_parse_expression(parser);
            if (node && parser->tokeniser.token == ITOK_RPAREN) {
                ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
            } else if (node) {
                ip_error_near
                    (parser, "missing \")\" in string index expression");
            }
            node = ip_ast_make_array_access
                (var, node, &(parser->tokeniser.loc));
        } else {
            node = ip_ast_make_variable(var, &(parser->tokeniser.loc));
        }
        break;

    case IP_TYPE_ARRAY_OF_INT:
    case IP_TYPE_ARRAY_OF_FLOAT:
    case IP_TYPE_ARRAY_OF_STRING:
        if (parser->tokeniser.token == ITOK_LPAREN &&
                (allowed & IP_VAR_ALLOW_ARRAYS) != 0) {
            /* Parse the array index expression */
            ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
            node = ip_parse_expression(parser);
            if (node && parser->tokeniser.token == ITOK_RPAREN) {
                ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
            } else if (node) {
                ip_error_near
                    (parser, "missing \")\" in array index expression");
            }

            /* Construct a lookup of the specific array index */
            node = ip_ast_make_array_access
                (var, node, &(parser->tokeniser.loc));
        } else if ((allowed & IP_VAR_ALLOW_ARRAYS) == 0) {
            ip_error_near
                (parser, "array variable '%s' is not permitted here",
                 ip_var_get_name(var));
        } else {
            ip_error_near(parser, "array index expected", ip_var_get_name(var));
        }
        break;

    default:
        ip_error(parser, "variable '%s' is not permitted here",
                 ip_var_get_name(var));
        break;
    }
    return node;
}

/*
 * UnaryExpression ::=
 *      {"+" | "-"} BaseExpression
 *    | "LENGTH OF" BaseExpression
 * BaseExpression ::=
 *      "THIS"
 *    | NUMBER
 *    | STRING
 *    | Variable
 *    | "(" Expression ")"
 *    | LocalVariable
 * LocalVariable ::=
 *      "@1" | "@2" | "@3" | "@4" | "@5" | "@6" | "@7" | "@8" | "@9"
 */
static ip_ast_node_t *ip_parse_unary_expression(ip_parser_t *parser)
{
    ip_ast_node_t *node;
    int is_neg = 0;
    int token;

    /* Deal with '+' or '-' prefixes to figure out if the term is negated */
    token = parser->tokeniser.token;
    while (token == ITOK_PLUS || token == ITOK_MINUS) {
        if (token == ITOK_MINUS) {
            is_neg = !is_neg;
            ip_parse_get_next
                (parser, ITOK_TYPE_EXPRESSION | ITOK_TYPE_NEGATIVE);
        } else {
            ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        }
        token = parser->tokeniser.token;
    }

    /* Parse the base expression */
    switch (token) {
    case ITOK_THIS:
        /* Type "THIS" based on the last value it had.  Within a block,
         * it is usually possible to guess the type of "THIS" from how the
         * previous statements used it.  If this is the first reference to
         * "THIS" in a block, the type will be "dynamic". */
        node = ip_ast_make_this(parser->this_type, &(parser->tokeniser.loc));
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        break;

    case ITOK_VAR_NAME:
    case ITOK_ARG_NUMBER:
        /* Parse the variable name, optionally followed by an array index */
        node = ip_parse_variable_expression
            (parser, IP_VAR_ALLOW_ARRAYS | IP_VAR_ALLOW_RVALUES |
                     IP_VAR_ALLOW_LOCALS);
        break;

    case ITOK_INT_VALUE:
        /* Recognise an integer constant */
        if (is_neg) {
            node = ip_ast_make_int_constant
                (-((ip_int_t)(parser->tokeniser.ivalue)),
                 &(parser->tokeniser.loc));
            is_neg = 0; /* No need to negate again below */
        } else {
            node = ip_ast_make_int_constant
                ((ip_int_t)(parser->tokeniser.ivalue),
                 &(parser->tokeniser.loc));
        }
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        break;

    case ITOK_FLOAT_VALUE:
        /* Recognise a floating-point constant */
        if (is_neg) {
            node = ip_ast_make_float_constant
                (-parser->tokeniser.fvalue, &(parser->tokeniser.loc));
            is_neg = 0; /* No need to negate again below */
        } else {
            node = ip_ast_make_float_constant
                (parser->tokeniser.fvalue, &(parser->tokeniser.loc));
        }
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        break;

    case ITOK_STR_VALUE:
        /* Recognise a string constant */
        if (is_neg) {
            ip_error(parser, "string negation is not permitted");
        }
        node = ip_ast_make_text
            (token, parser->tokeniser.token_info->name,
             &(parser->tokeniser.loc));
        node->value_type = IP_TYPE_STRING;
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        break;

    case ITOK_LPAREN:
        /* Parse an expression in parentheses */
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        node = ip_parse_extended_expression(parser);
        if (parser->tokeniser.token == ITOK_RPAREN) {
            ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        } else {
            ip_error_near(parser, "missing \")\" in parenthesised expression");
        }
        break;

    case ITOK_LENGTH_OF:
        /* Operator to get the length of a string or an array */
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        if (parser->tokeniser.token == ITOK_VAR_NAME) {
            /* Is this variable an array name with no indexing?
             * If so, then we want the length of the array. */
            ip_var_t *var = ip_var_lookup
                (&(parser->program->vars), parser->tokeniser.token_info->name);
            if (var && ip_var_is_array(var) &&
                    !ip_tokeniser_lookahead(&(parser->tokeniser), '(')) {
                ip_int_t length =
                    var->max_subscript - var->min_subscript + 1;
                node = ip_ast_make_int_constant
                    (length, &(parser->tokeniser.loc));
                ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
                break;
            }
        }
        node = ip_parse_unary_expression(parser);
        if (node && node->value_type != IP_TYPE_STRING) {
            ip_error_at(parser, &(node->loc),
                        "string value expected for 'LENGTH OF'");
        }
        node = ip_ast_make_unary
            (token, node, &(parser->tokeniser.loc));
        break;

    default:
        ip_error_near(parser, "variable name or constant expected");
        node = 0;
        break;
    }

    /* Negate the base expression by subtracting from zero if necessary */
    if (is_neg && node) {
        node = ip_parse_negate_node(parser, node);
    }
    return node;
}

/**
 * @brief Check if a node is numeric and complain if it is not.
 *
 * @param[in,out] parser The parse state.
 * @param[in] node The node to check.
 *
 * @return Non-zero if the node is numeric, or zero if it is a string.
 */
static int ip_parse_numeric_check(ip_parser_t *parser, ip_ast_node_t *node)
{
    if (node && node->value_type == IP_TYPE_STRING) {
        ip_error_at(parser, &(node->loc),
                    "strings are not permitted in this type of expression");
        return 0;
    }
    return 1;
}

/*
 * MultiplicativeExpression ::=
 *      UnaryExpression
 *    | MultiplicativeExpression MulOp UnaryExpression
 * MulOp ::= "*" | "/" | "MODULO"
 */
static ip_ast_node_t *ip_parse_multiplicative_expression(ip_parser_t *parser)
{
    ip_ast_node_t *node = ip_parse_unary_expression(parser);
    ip_ast_node_t *node2;
    int token = parser->tokeniser.token;
    while (token == ITOK_MUL || token == ITOK_DIV || token == ITOK_MODULO) {
        if (!ip_parse_numeric_check(parser, node)) {
            ip_ast_node_free(node);
            node = 0;
        }
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        node2 = ip_parse_unary_expression(parser);
        if (node && !ip_parse_numeric_check(parser, node2)) {
            ip_ast_node_free(node2);
            node2 = 0;
        }
        node = ip_ast_make_binary(token, node, node2, &(parser->tokeniser.loc));
        token = parser->tokeniser.token;
    }
    return node;
}

/*
 * Expression ::= AdditiveExpression
 * AdditiveExpression ::=
 *      MultiplicativeExpression
 *    | AdditiveExpression AddOp MultiplicativeExpression
 * AddOp ::= "+" | "-"
 */
static ip_ast_node_t *ip_parse_extended_expression(ip_parser_t *parser)
{
    ip_ast_node_t *node = ip_parse_multiplicative_expression(parser);
    ip_ast_node_t *node2;
    int token = parser->tokeniser.token;
    while (token == ITOK_PLUS || token == ITOK_MINUS) {
        if (token != ITOK_PLUS && !ip_parse_numeric_check(parser, node)) {
            ip_ast_node_free(node);
            node = 0;
        }
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        node2 = ip_parse_multiplicative_expression(parser);
        if (token != ITOK_PLUS && node &&
                !ip_parse_numeric_check(parser, node2)) {
            ip_ast_node_free(node2);
            node2 = 0;
        }
        node = ip_ast_make_binary(token, node, node2, &(parser->tokeniser.loc));
        token = parser->tokeniser.token;
    }
    return node;
}

/*
 * ClassicExpression ::=
 *      NUMBER
 *    | "-" NUMBER
 *    | VAR-NAME
 *    | "-" VAR-NAME
 *    | VAR-NAME "+" NUMBER
 *    | VAR-NAME "-" NUMBER
 *
 * Technically, classic floating-point expressions can only be
 * NUMBER or -NUMBER.  We allow some extra forms for simplicity.
 */
static ip_ast_node_t *ip_parse_classic_expression(ip_parser_t *parser)
{
    ip_ast_node_t *node = 0;
    ip_ast_node_t *node2;
    int token;

    switch (parser->tokeniser.token) {
    case ITOK_THIS:
        /* Type "THIS" based on the last value it had.  Within a block,
         * it is usually possible to guess the type of "THIS" from how the
         * previous statements used it.  If this is the first reference to
         * "THIS" in a block, the type will be "dynamic". */
        node = ip_ast_make_this(parser->this_type, &(parser->tokeniser.loc));
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        break;

    case ITOK_INT_VALUE:
        /* Recognise a positive integer constant */
        node = ip_ast_make_int_constant
            ((ip_int_t)(parser->tokeniser.ivalue), &(parser->tokeniser.loc));
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        break;

    case ITOK_FLOAT_VALUE:
        /* Recognise a positive floating-point constant */
        node = ip_ast_make_float_constant
            (parser->tokeniser.fvalue, &(parser->tokeniser.loc));
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        break;

    case ITOK_VAR_NAME:
        /* Variable name which may be followed by "+ N" or "- N" */
        node = ip_parse_variable_expression
            (parser, IP_VAR_ALLOW_RVALUES);
        token = parser->tokeniser.token;
        if (token == ITOK_PLUS || token == ITOK_MINUS) {
            ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
            if (parser->tokeniser.token == ITOK_INT_VALUE) {
                node2 = ip_ast_make_int_constant
                    ((ip_int_t)(parser->tokeniser.ivalue),
                     &(parser->tokeniser.loc));
                ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
            } else if (parser->tokeniser.token == ITOK_FLOAT_VALUE) {
                node2 = ip_ast_make_float_constant
                    (parser->tokeniser.fvalue, &(parser->tokeniser.loc));
                ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
            } else {
                ip_error_near(parser, "number expected");
                node2 = 0;
            }
            node = ip_ast_make_binary
                (token, node, node2, &(parser->tokeniser.loc));
        }
        break;

    case ITOK_MINUS:
        /* Negated constant or variable */
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION | ITOK_TYPE_NEGATIVE);
        switch (parser->tokeniser.token) {
        case ITOK_INT_VALUE:
            /* Recognise a negative integer constant */
            node = ip_ast_make_int_constant
                (-((ip_int_t)(parser->tokeniser.ivalue)),
                 &(parser->tokeniser.loc));
            ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
            break;

        case ITOK_FLOAT_VALUE:
            /* Recognise a negative floating-point constant */
            node = ip_ast_make_float_constant
                (-(parser->tokeniser.fvalue), &(parser->tokeniser.loc));
            ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
            break;

        case ITOK_VAR_NAME:
            /* Name of a variable which is being negated */
            node = ip_parse_variable_expression
                (parser, IP_VAR_ALLOW_RVALUES);
            node = ip_parse_negate_node(parser, node);
            break;

        default:
            /* Unknown token - this is an error */
            ip_error_near(parser, "variable name or number expected");
            break;
        }
        break;

    default:
        /* Unknown token - this is an error */
        ip_error_near(parser, "variable name or number expected");
        break;
    }
    return node;
}

ip_ast_node_t *ip_parse_expression(ip_parser_t *parser)
{
    if ((parser->flags & ITOK_TYPE_EXTENSION) == 0) {
        /* Classic INTERPROGRAM only allows a few expression forms */
        return ip_parse_classic_expression(parser);
    } else {
        /* Extended INTERPROGRAM allows more expression forms */
        return ip_parse_extended_expression(parser);
    }
}

static ip_ast_node_t *ip_parse_next_expression(ip_parser_t *parser)
{
    ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
    return ip_parse_expression(parser);
}

static ip_ast_node_t *ip_parse_next_integer_expression(ip_parser_t *parser)
{
    ip_ast_node_t *node = ip_parse_next_expression(parser);
    return ip_ast_make_cast(IP_TYPE_INT, node);
}

static ip_ast_node_t *ip_parse_next_float_expression(ip_parser_t *parser)
{
    ip_ast_node_t *node = ip_parse_next_expression(parser);
    return ip_ast_make_cast(IP_TYPE_FLOAT, node);
}

/*
 * Condition ::=
 *      Expression BinaryConditionOperator Expression
 *    | Expression UnaryConditionOperator
 *
 * BinaryConditionOperator ::=
 *      "IS GREATER THAN"
 *    | "IS MUCH GREATER THAN"
 *    | "IS SMALLER THAN"
 *    | "IS MUCH SMALLER THAN"
 *    | "IS EQUAL TO"                   # Extensions
 *    | "IS NOT EQUAL TO"
 *    | "IS GREATER THAN OR EQUAL TO"
 *    | "IS SMALLER THAN OR EQUAL TO"
 *    | "IS EMPTY"
 *    | "IS NOT EMPTY"
 *
 * UnaryConditionOperator ::=
 *      "IS ZERO"
 *    | "IS POSITIVE"
 *    | "IS NEGATIVE"
 *    | "IS NOT ZERO"                   # Extensions
 *    | "IS FINITE"
 *    | "IS INFINITE"
 *    | "IS NOT A NUMBER"
 */
static ip_ast_node_t *ip_parse_condition(ip_parser_t *parser)
{
    ip_ast_node_t *node;
    ip_ast_node_t *node2;
    int token;
    int is_condition;

    /* Parse the first expression in the conditional */
    node = ip_parse_next_expression(parser);

    /* Expect either "IS" or "IS NOT" to appear next */
    token = parser->tokeniser.token;
    if (token == ITOK_IS) {
        is_condition = 1;
    } else if (token == ITOK_IS_NOT) {
        is_condition = 0;
    } else {
        if ((parser->flags & ITOK_TYPE_EXTENSION) == 0) {
            ip_error_near(parser, "'IS' expected");
        } else {
            ip_error_near(parser, "'IS' or 'IS NOT' expected");
        }
        ip_ast_node_free(node);
        return 0;
    }
    ip_parse_get_next(parser, ITOK_TYPE_CONDITION);

    /* Determine what kind of condition we have */
    token = parser->tokeniser.token;
    switch (token) {
    case ITOK_GREATER_THAN:
    case ITOK_GREATER_OR_EQUAL:
    case ITOK_MUCH_GREATER_THAN:
    case ITOK_SMALLER_THAN:
    case ITOK_SMALLER_OR_EQUAL:
    case ITOK_MUCH_SMALLER_THAN:
    case ITOK_EQUAL_TO:
        node2 = ip_parse_next_expression(parser);
        node = ip_ast_make_binary
            (token, node, node2, &(parser->tokeniser.loc));
        if (is_condition) {
            node = ip_ast_make_unary
                (ITOK_IS, node, &(parser->tokeniser.loc));
        } else {
            node = ip_ast_make_unary
                (ITOK_IS_NOT, node, &(parser->tokeniser.loc));
        }
        if (node) {
            /* Conditions always have a boolean result */
            node->value_type = IP_TYPE_INT;
        }
        break;

    case ITOK_ZERO:
    case ITOK_POSITIVE:
    case ITOK_NEGATIVE:
    case ITOK_FINITE:
    case ITOK_INFINITE:
    case ITOK_A_NUMBER:
    case ITOK_EMPTY:
        node = ip_ast_make_unary(token, node, &(parser->tokeniser.loc));
        if (is_condition) {
            node = ip_ast_make_unary
                (ITOK_IS, node, &(parser->tokeniser.loc));
        } else {
            node = ip_ast_make_unary
                (ITOK_IS_NOT, node, &(parser->tokeniser.loc));
        }
        if (node) {
            /* Conditions always have a boolean result */
            node->value_type = IP_TYPE_INT;
        }
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        break;

    default:
        ip_error_near(parser, "invalid condition");
        node = 0;
        break;
    }
    return node;
}

/*
 * IfStatement ::=
 *      "IF" Condition
 *    | "IF" Condition "THEN" Statements { ElseIfClause } [ ElseClause ] EndIf
 */
static ip_ast_node_t *ip_parse_if_statement(ip_parser_t *parser)
{
    ip_ast_node_t *node;

    /* Parse the basic "IF" statement form */
    node = ip_parse_condition(parser);
    if (parser->tokeniser.token != ITOK_THEN) {
        return ip_ast_make_unary_statement
            (ITOK_IF, IP_TYPE_UNKNOWN, node, &(parser->tokeniser.loc));
    }

    /* Next token is "THEN", so we are doing a fully-structured "IF" */
    node = ip_ast_make_unary_statement
        (ITOK_THEN, IP_TYPE_UNKNOWN, node, &(parser->tokeniser.loc));
    ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
    if (node) {
        ip_parse_create_block(parser, ITOK_IF, node);
    }
    return node;
}

/*
 * ElseIfClause ::= "ELSE IF" Condition "THEN" Statements
 */
static ip_ast_node_t *ip_parse_else_if(ip_parser_t *parser)
{
    ip_ast_node_t *node = ip_parse_condition(parser);
    if (parser->tokeniser.token != ITOK_THEN) {
        ip_error_near(parser, "'THEN' expected");
    } else {
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
    }
    node = ip_ast_make_unary_statement
        (ITOK_ELSE_IF, IP_TYPE_UNKNOWN, node, &(parser->tokeniser.loc));
    if (!node) {
        return node;
    }
    if (!parser->blocks || parser->blocks->type != ITOK_IF) {
        ip_error(parser, "'ELSE IF' without a matching 'IF'");
    } else if (parser->blocks->patch->type == ITOK_ELSE) {
        ip_error(parser, "'ELSE IF' after 'ELSE'");
    } else {
        /* Backpatch the 'IF' to add the 'ELSE IF' clause */
        parser->blocks->patch->children.right = node;
        parser->blocks->patch->dont_free_right = 1;
        parser->blocks->patch = node;
    }
    return node;
}

/*
 * ElseClause ::= "ELSE" Statements
 */
static ip_ast_node_t *ip_parse_else(ip_parser_t *parser)
{
    ip_ast_node_t *node;
    ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
    node = ip_ast_make_standalone(ITOK_ELSE, &(parser->tokeniser.loc));
    if (!parser->blocks || parser->blocks->type != ITOK_IF) {
        ip_error(parser, "'ELSE' without a matching 'IF'");
    } else if (parser->blocks->patch->type == ITOK_ELSE) {
        ip_error(parser, "multiple 'ELSE' clauses in an 'IF' statement");
    } else {
        /* Backpatch the 'IF' to add the 'ELSE' clause */
        parser->blocks->patch->children.right = node;
        parser->blocks->patch->dont_free_right = 1;
        parser->blocks->patch = node;
    }
    return node;
}

/*
 * EndIf ::= "END IF"
 */
static ip_ast_node_t *ip_parse_end_if(ip_parser_t *parser)
{
    ip_ast_node_t *node;
    ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
    node = ip_ast_make_standalone(ITOK_END_IF, &(parser->tokeniser.loc));
    if (!parser->blocks || parser->blocks->type != ITOK_IF) {
        ip_error(parser, "'END IF' without a matching 'IF'");
    } else {
        /* Backpatch the 'IF' to add the 'END IF' */
        parser->blocks->patch->children.right = node;
        parser->blocks->patch->dont_free_right = 1;

        /* Pop the 'IF' block from the context stack */
        ip_parse_free_top_block(parser);
    }
    return node;
}

/*
 * WhileStatement ::= "REPEAT WHILE" Condition
 */
static ip_ast_node_t *ip_parse_while_statement(ip_parser_t *parser)
{
    ip_ast_node_t *node = ip_parse_condition(parser);
    node = ip_ast_make_unary_statement
        (ITOK_REPEAT_WHILE, IP_TYPE_UNKNOWN, node, &(parser->tokeniser.loc));
    ip_parse_create_block(parser, ITOK_REPEAT_WHILE, node);
    return node;
}

/*
 * EndRepeatStatement ::= "END REPEAT"
 */
static ip_ast_node_t *ip_parse_end_repeat_statement(ip_parser_t *parser)
{
    ip_ast_node_t *node;
    ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
    node = ip_ast_make_standalone(ITOK_END_REPEAT, &(parser->tokeniser.loc));
    if (!parser->blocks || parser->blocks->type != ITOK_REPEAT_WHILE) {
        ip_error(parser, "'END REPEAT' without a matching 'REPEAT'");
    } else {
        /* Backpatch the 'REPEAT WHILE' to point at the 'END REPEAT'
         * node and point the 'END REPEAT' node at the 'REPEAT WHILE'. */
        ip_ast_node_t *repeat = parser->blocks->patch;
        repeat->children.right = node;
        repeat->dont_free_right = 1;
        node->has_children = 1;
        node->dont_free_right = 1;
        node->children.right = repeat;

        /* Pop the 'REPEAT WHILE' block from the context stack */
        ip_parse_free_top_block(parser);
    }
    return node;
}

/**
 * @brief Verify that a string being used as a label is valid.
 *
 * @param[in,out] parser The parsing context.
 *
 * @return Non-zero if the string is valid, or zero if not.
 *
 * Label names must be a sequence of words separated by single spaces.
 * The words must be exclusively made up of letters A-Z or a-z.
 */
static int ip_parse_verify_label_string(ip_parser_t *parser)
{
    char *name = parser->tokeniser.name;
    int saw_letters = 0;
    int last_was_letter = 0;
    int ch;

    /* Convert the string to upper case and check for invalid characters */
    while ((ch = *name) != '\0') {
        if (ch >= 'A' && ch <= 'Z') {
            saw_letters = 1;
            last_was_letter = 1;
        } else if (ch >= 'a' && ch <= 'z') {
            *name = ch - 'a' + 'A';
            saw_letters = 1;
            last_was_letter = 1;
        } else if (ch == ' ' && last_was_letter && name[1] != '\0') {
            last_was_letter = 0;
        } else {
            return 0;
        }
        ++name;
    }

    /* The name must not correspond to a built-in keyword */
    if (ip_tokeniser_lookup_keyword
            (parser->tokeniser.name, strlen(parser->tokeniser.name),
             parser->flags) != 0) {
        return 0;
    }

    /* Must have at least one letter in the name */
    return saw_letters;
}

/*
 * LabelName ::=
 *      "*" Expression
 *    | VAR-NAME                        # Extensions
 *    | STRING
 */
static ip_ast_node_t *ip_parse_label_name(ip_parser_t *parser)
{
    ip_ast_node_t *node = 0;
    ip_label_t *label = 0;
    const char *name;
    if (parser->tokeniser.token == ITOK_LABEL) {
        /* Numeric or computed label */
        node = ip_parse_next_expression(parser);
        if (node && node->type == ITOK_INT_VALUE &&
                node->ivalue >= IP_MIN_LABEL_NUMBER &&
                node->ivalue <= IP_MAX_LABEL_NUMBER) {
            /* We can replace this with a direct label reference.
             * No need to compute the label number at runtime. */
            ip_int_t num = node->ivalue;
            ip_ast_node_free(node);
            node = 0;
            label = ip_label_lookup_by_number(&(parser->program->labels), num);
            if (!label) {
                label = ip_label_create_by_number
                    (&(parser->program->labels), num);
            }
        }
    } else if ((parser->tokeniser.token == ITOK_VAR_NAME ||
                parser->tokeniser.token == ITOK_ROUTINE_NAME) &&
               (parser->flags & ITOK_TYPE_EXTENSION) != 0) {
        /* Named label */
        name = parser->tokeniser.token_info->name;
        label = ip_label_lookup_by_name(&(parser->program->labels), name);
        if (!label) {
            label = ip_label_create_by_name
                (&(parser->program->labels), name);
        }
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
    } else if (parser->tokeniser.token == ITOK_STR_VALUE &&
               (parser->flags & ITOK_TYPE_EXTENSION) != 0) {
        /* Named label using a string */
        if (ip_parse_verify_label_string(parser)) {
            name = parser->tokeniser.token_info->name;
            label = ip_label_lookup_by_name(&(parser->program->labels), name);
            if (!label) {
                label = ip_label_create_by_name
                    (&(parser->program->labels), name);
            }
        } else {
            ip_error(parser, "invalid label string");
        }
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
    } else if ((parser->flags & ITOK_TYPE_EXTENSION) != 0) {
        ip_error_near(parser, "label number or name expected");
    } else {
        ip_error_near(parser, "label number expected");
    }
    if (label) {
        node = ip_ast_make_standalone(ITOK_LABEL, &(parser->tokeniser.loc));
        node->label = label;
    }
    return node;
}

/*
 * CallArguments ::= Expression { ":" Expression }
 */
static ip_ast_node_t *ip_parse_call_arguments
    (ip_parser_t *parser, ip_ast_node_t *call)
{
    ip_ast_node_t *list = 0;
    ip_ast_node_t *arg;
    ip_int_t count = 0;
    for (;;) {
        arg = ip_parse_expression(parser);
        if (!arg) {
            /* An error occurred while parsing the argument */
            ip_ast_node_free(list);
            ip_ast_node_free(call);
            return 0;
        }
        arg = ip_ast_make_argument
            (ITOK_SET, count, arg, &(parser->tokeniser.loc));
        ++count;
        if (count == (IP_MAX_LOCALS + 1)) {
            ip_error(parser, "too many arguments to subroutine call, max %d",
                     IP_MAX_LOCALS);
        }
        if (list) {
            list = ip_ast_make_binary
                (ITOK_ARG_LIST, list, arg, &(parser->tokeniser.loc));
        } else {
            list = arg;
        }
        if (parser->tokeniser.token == ITOK_COLON) {
            /* Check for the ":" separator between arguments */
            ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        } else {
            break;
        }
    }
    if (!list) {
        ip_ast_node_free(call);
        return 0;
    }
    call->children.right = list;
    return call;
}

static ip_ast_node_t *ip_parse_extract_substring(ip_parser_t *parser)
{
    ip_ast_node_t *from;
    ip_ast_node_t *node;

    /* Skip "SUBSTRING FROM" and parse the starting index */
    from = ip_parse_next_integer_expression(parser);

    /* If the next token is "TO", then we also have an ending index.
     * Otherwise extract everything from the starting index onwards. */
    if (parser->tokeniser.token == ITOK_TO) {
        node = ip_parse_next_integer_expression(parser);
        node = ip_ast_make_binary_statement
            (ITOK_SUBSTRING, IP_TYPE_STRING, from, node,
             &(parser->tokeniser.loc));
    } else {
        node = ip_ast_make_unary_statement
            (ITOK_SUBSTRING, IP_TYPE_STRING, from,
             &(parser->tokeniser.loc));
    }
    return node;
}

/**
 * @brief Determine if a token terminates the current line.
 *
 * @param[in] token The token to test.
 *
 * @return Non-zero if @a token terminates the current line.
 */
static int ip_parse_token_is_terminator(int token)
{
    return token == ITOK_EOF || token == ITOK_EOL || token == ITOK_INPUT_DATA;
}

/*
 * Statement ::=
 *      AssignmentStatement
 *    | ArithmeticStatement
 *    | BitwiseStatement
 *    | MathFunctionStatement
 *    | IfStatement
 *    | ControlFlowStatement
 *    | InputOutputStatement
 *    | StringStatement
 *    | "&" ...
 *
 * AssignmentStatement ::=
 *      "TAKE" Expression
 *    | "REPLACE" Variable
 *    | "SET" Variable "=" Expression
 *
 * ArithmeticStatement ::=
 *      "ADD" Expression
 *    | "SUBTRACT" Expression
 *    | "MULTIPLY BY" Expression
 *    | "DIVIDE BY" Expression
 *    | "MODULO" Expression             # Extension
 *
 * BitwiseStatement ::=                 # Extensions
 *      "BITWISE AND WITH" Expression
 *    | "BITWISE AND WITH NOT" Expression
 *    | "BITWISE OR WITH" Expression
 *    | "BITWISE XOR WITH" Expression
 *    | "BITWISE NOT"
 *    | "SHIFT LEFT BY" Expression
 *    | "SHIFT RIGHT BY" Expression
 *
 * MathFunctionStatement ::=
 *      "FORM SQUARE ROOT"
 *    | "FORM SINE"
 *    | "FORM COSINE"
 *    | "FORM TANGENT"
 *    | "FORM ARCTAN"
 *    | "FORM NATURAL LOG"
 *    | "FORM EXPONENTIAL"
 *    | "FORM ABSOLUTE"                 # Extensions
 *    | "RAISE TO THE POWER OF" Expression
 *    | "FORM SINE RADIANS"
 *    | "FORM COSINE RADIANS"
 *    | "FORM TANGENT RADIANS"
 *    | "FORM ARCTAN RADIANS"
 *    | "FORM SINE DEGREES"
 *    | "FORM COSINE DEGREES"
 *    | "FORM TANGENT DEGREES"
 *    | "FORM ARCTAN DEGREES"
 *    | "RANDOM NUMBER"
 *    | "SEED RANDOM" Expression
 *    | "ROUND NEAREST"
 *    | "ROUND UP"
 *    | "ROUND DOWN"
 *
 * ControlFlowStatement ::=
 *      "GO TO" LabelName
 *    | "EXECUTE PROCESS" LabelName [CallArguments]
 *    | "REPEAT FROM" LabelName VAR-NAME "TIMES"
 *    | "END OF PROCESS DEFINITION"
 *    | "END OF INTERPROGRAM"
 *    | "PAUSE" Expression
 *    | "CALL" LabelName [CallArguments]    # Extensions
 *    | "RETURN"
 *    | "RETURN" Expression
 *    | "EXIT INTERPROGRAM"
 *    | WhileStatement
 *    | EndRepeatStatement
 *
 * InputOutputStatement ::=
 *      "INPUT" Variable
 *    | "INPUT"
 *    | "OUTPUT" Expression
 *    | "OUTPUT"
 *    | "PUNCH THE FOLLOWING CHARACTERS" EOL TEXT BLANKS
 *    | "COPY TAPE"
 *    | "IGNORE TAPE"
 *
 * StringStatement ::=
 *      "SUBSTRING FROM" Expression [ "TO" Expression ]
 *    | "LENGTH OF"
 */
static ip_ast_node_t *ip_parse_statement(ip_parser_t *parser)
{
    int token = parser->tokeniser.token;
    ip_ast_node_t *node = 0;
    ip_ast_node_t *var;
    ip_label_t *label;
    char *text;

    /* Handle the "&" repetition construct */
    if (token == ITOK_AMPERSAND) {
        if (parser->last_statement == -1) {
            ip_error(parser, "no statement to repeat with '&'");
            return 0;
        }
        token = parser->last_statement;
    }
    parser->last_statement = -1;

    /* Figure out what kind of statement we have */
    switch (token) {
    case ITOK_COMMA:
    case ITOK_EOL:
    case ITOK_EOF:
        /* Empty statement */
        return 0;

    /* ------------- Assignment statements ------------- */

    case ITOK_TAKE:
        /* TAKE expression */
        node = ip_parse_next_expression(parser);
        if (node) {
            node = ip_ast_make_unary_statement
                (ITOK_TAKE, node->value_type, node,
                 &(parser->tokeniser.loc));
        }
        break;

    case ITOK_REPLACE:
        /* REPLACE variable */
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        if ((parser->flags & ITOK_TYPE_EXTENSION) != 0) {
            var = ip_parse_variable_expression
                (parser, IP_VAR_ALLOW_ARRAYS | IP_VAR_ALLOW_LOCALS);
        } else {
            var = ip_parse_variable_expression(parser, IP_VAR_ALLOW_ARRAYS);
        }
        node = ip_ast_make_unary_statement
            (ITOK_REPLACE, IP_TYPE_UNKNOWN, var, &(parser->tokeniser.loc));
        break;

    case ITOK_SET:
        /* SET variable = expression */
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        if ((parser->flags & ITOK_TYPE_EXTENSION) != 0) {
            var = ip_parse_variable_expression
                (parser, IP_VAR_ALLOW_ARRAYS | IP_VAR_ALLOW_LOCALS);
        } else {
            var = ip_parse_variable_expression(parser, IP_VAR_ALLOW_ARRAYS);
        }
        if (parser->tokeniser.token == ITOK_EQUAL) {
            /* Parse the expresion */
            node = ip_parse_next_expression(parser);

            /* Cast the expression to the type of the variable */
            if (var) {
                node = ip_ast_make_cast(var->value_type, node);
            }

            /* Construct the "SET" statement */
            node = ip_ast_make_binary_statement
                (ITOK_SET, IP_TYPE_UNKNOWN, var, node,
                 &(parser->tokeniser.loc));
        } else {
            ip_error_near(parser, "'=' expected");
            ip_ast_node_free(var);
            node = 0;
        }
        break;

    /* ------------- Arithmetic statements ------------- */

    case ITOK_ADD:
    case ITOK_SUBTRACT:
    case ITOK_MULTIPLY:
    case ITOK_DIVIDE:
    case ITOK_MODULO:
        node = ip_ast_make_this_binary
            (token, parser->this_type, IP_TYPE_UNKNOWN,
             ip_parse_next_expression(parser),
             &(parser->tokeniser.loc));
        break;

    /* ------------- Bitwise statements ------------- */

    case ITOK_BITWISE_AND:
    case ITOK_BITWISE_AND_NOT:
    case ITOK_BITWISE_OR:
    case ITOK_BITWISE_XOR:
    case ITOK_SHIFT_LEFT:
    case ITOK_SHIFT_RIGHT:
        node = ip_ast_make_this_binary
            (token, parser->this_type, IP_TYPE_INT,
             ip_parse_next_integer_expression(parser),
             &(parser->tokeniser.loc));
        break;

    case ITOK_BITWISE_NOT:
        node = ip_ast_make_this_unary
            (token, parser->this_type, IP_TYPE_INT, &(parser->tokeniser.loc));
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        break;

    /* ------------- Mathematical function statements ------------- */

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
    case ITOK_ROUND_NEAREST:
    case ITOK_ROUND_UP:
    case ITOK_ROUND_DOWN:
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        node = ip_ast_make_this_unary
            (token, parser->this_type, IP_TYPE_FLOAT,
             &(parser->tokeniser.loc));
        break;

    case ITOK_RAISE:
        node = ip_ast_make_this_binary
            (token, parser->this_type, IP_TYPE_FLOAT,
             ip_parse_next_float_expression(parser),
             &(parser->tokeniser.loc));
        break;

    case ITOK_RANDOM:
        node = ip_ast_make_standalone(token, &(parser->tokeniser.loc));
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        break;

    case ITOK_SEED_RANDOM:
        /* SEED RANDOM expression */
        node = ip_parse_next_expression(parser);
        if (node) {
            node = ip_ast_make_unary_statement
                (token, IP_TYPE_UNKNOWN, node, &(parser->tokeniser.loc));
        }
        break;

    /* ------------- Conditional statements ------------- */

    case ITOK_IF:
        node = ip_parse_if_statement(parser);
        break;

    case ITOK_ELSE_IF:
        node = ip_parse_else_if(parser);
        break;

    case ITOK_ELSE:
        node = ip_parse_else(parser);
        break;

    case ITOK_END_IF:
        node = ip_parse_end_if(parser);
        break;

    /* ------------- Control flow statements ------------- */

    case ITOK_GO_TO:
        /* GO TO label */
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        node = ip_parse_label_name(parser);
        node = ip_ast_make_unary_statement
            (token, IP_TYPE_DYNAMIC, node, &(parser->tokeniser.loc));
        break;

    case ITOK_EXECUTE_PROCESS:
    case ITOK_CALL:
        /* CALL label [arguments] */
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        node = ip_parse_label_name(parser);
        node = ip_ast_make_unary_statement
            (ITOK_CALL, IP_TYPE_DYNAMIC, node, &(parser->tokeniser.loc));
        if (!ip_parse_token_is_terminator(parser->tokeniser.token) &&
                parser->tokeniser.token != ITOK_COMMA &&
                (parser->flags & ITOK_TYPE_EXTENSION) != 0) {
            /* Statement is followed by a set of colon-separated arguments */
            node = ip_parse_call_arguments(parser, node);
        }
        break;

    case ITOK_VAR_NAME:
    case ITOK_ROUTINE_NAME:
        /* This might be the name of a routine that was declared with
         * "SYMBOLS FOR ROUTINES".  We can skip the "CALL" if it is. */
        label = ip_label_lookup_by_name
            (&(parser->program->labels), parser->tokeniser.name);
        if (label && label->base.type == IP_TYPE_ROUTINE) {
            node = ip_parse_label_name(parser);
            node = ip_ast_make_unary_statement
                (ITOK_CALL, IP_TYPE_DYNAMIC, node, &(parser->tokeniser.loc));
            if (!ip_parse_token_is_terminator(parser->tokeniser.token) &&
                    parser->tokeniser.token != ITOK_COMMA &&
                    (parser->flags & ITOK_TYPE_EXTENSION) != 0) {
                /* Statement is followed by colon-separated arguments */
                node = ip_parse_call_arguments(parser, node);
            }
        } else {
            ip_error_near(parser, 0);
        }
        break;

    case ITOK_REPEAT_FROM:
        /* REPEAT FROM *expression variable TIMES */
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        node = ip_parse_label_name(parser);
        var = ip_parse_variable_expression(parser, 0);
        if (var && var->value_type != IP_TYPE_INT) {
            ip_error_at
                (parser, &(var->loc),
                 "integer variable required for loops");
        }
        node = ip_ast_make_binary_statement
            (token, IP_TYPE_DYNAMIC, node, var, &(parser->tokeniser.loc));
        if (parser->tokeniser.token == ITOK_TIMES) {
            ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
            if ((parser->flags & ITOK_TYPE_EXTENSION) == 0) {
                /* Classic INTERPROGRAM requires that "REPEAT FROM"
                 * must be the last statement on a line. */
                if (!ip_parse_token_is_terminator(parser->tokeniser.token)) {
                    ip_error(parser, "end of line expected after 'REPEAT FROM' statement");
                }
            }
        } else {
            ip_error_near(parser, "'TIMES' expected");
        }
        break;

    case ITOK_END_PROCESS:
    case ITOK_END_PROGRAM:
    case ITOK_EXIT_PROGRAM:
        node = ip_ast_make_standalone(token, &(parser->tokeniser.loc));
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        break;

    case ITOK_RETURN:
        /*
         * RETURN
         * RETURN expression
         */
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        if (parser->tokeniser.token == ITOK_COMMA ||
                ip_parse_token_is_terminator(parser->tokeniser.token)) {
            /* "RETURN" with no value other than "THIS" */
            node = ip_ast_make_standalone
                (ITOK_RETURN, &(parser->tokeniser.loc));
            if (node) {
                /* Type of "THIS" is undefined after a "RETURN" */
                node->this_type = IP_TYPE_DYNAMIC;
            }
        } else {
            /* "RETURN" with a value that is copied into "THIS" */
            node = ip_parse_expression(parser);
            node = ip_ast_make_unary_statement
                (ITOK_RETURN, IP_TYPE_DYNAMIC, node, &(parser->tokeniser.loc));
        }
        break;

    case ITOK_REPEAT_WHILE:
        /* REPEAT WHILE Condition */
        node = ip_parse_while_statement(parser);
        break;

    case ITOK_END_REPEAT:
        /* END REPEAT */
        node = ip_parse_end_repeat_statement(parser);
        break;

    /* ------------- Input/Output statements ------------- */

    case ITOK_INPUT:
        /*
         * INPUT variable
         * INPUT
         */
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        if (parser->tokeniser.token == ITOK_VAR_NAME) {
            /* Input into the variable and "THIS" */
            if ((parser->flags & ITOK_TYPE_EXTENSION) != 0) {
                var = ip_parse_variable_expression
                    (parser, IP_VAR_ALLOW_ARRAYS | IP_VAR_ALLOW_LOCALS);
            } else {
                var = ip_parse_variable_expression(parser, IP_VAR_ALLOW_ARRAYS);
            }
        } else {
            /* Input into "THIS" only; always floating-point */
            var = ip_ast_make_this(IP_TYPE_FLOAT, &(parser->tokeniser.loc));
        }
        if (var) {
            /* The type of "THIS" is set to that of the variable because the
             * "INPUT" statement also side-effects "THIS". */
            node = ip_ast_make_unary_statement
                (ITOK_INPUT, var->value_type, var, &(parser->tokeniser.loc));
        }
        break;

    case ITOK_PAUSE:
        /* PAUSE expression */
        node = ip_parse_next_expression(parser);
        if (node) {
            node = ip_ast_make_unary_statement
                (ITOK_PAUSE, IP_TYPE_UNKNOWN, node,
                 &(parser->tokeniser.loc));
        }
        break;

    case ITOK_OUTPUT:
        /*
         * OUTPUT expression
         * OUTPUT
         */
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        if (parser->tokeniser.token == ITOK_COMMA ||
                ip_parse_token_is_terminator(parser->tokeniser.token)) {
            /* Output "THIS" with aligned formatting */
            node = ip_ast_make_standalone
                (ITOK_OUTPUT, &(parser->tokeniser.loc));
        } else {
            /* Output the value of an expression with no formatting */
            node = ip_parse_expression(parser);
            if (node) {
                node = ip_ast_make_unary_statement
                    (ITOK_OUTPUT, IP_TYPE_UNKNOWN, node,
                     &(parser->tokeniser.loc));
            }
        }
        break;

    case ITOK_PUNCH:
        if (parser->tokeniser.buffer_posn < parser->tokeniser.buffer_len) {
            /* If "PUNCH THE FOLLOWING CHARACTERS" is followed by a comma,
             * then suppress the output of "~~~~~" blanks at runtime.
             * We also do this in the extended syntax as blanks are useless. */
            if (parser->tokeniser.buffer[parser->tokeniser.buffer_posn] == ',') {
                ++(parser->tokeniser.buffer_posn);
                token = ITOK_PUNCH_NO_BLANKS;
            } else if ((parser->flags & ITOK_TYPE_EXTENSION) != 0) {
                token = ITOK_PUNCH_NO_BLANKS;
            }
        }
        text = ip_tokeniser_read_punch(&(parser->tokeniser));
        node = ip_ast_make_text(token, text, &(parser->tokeniser.loc));
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        break;

    case ITOK_COPY_TAPE:
        if (parser->tokeniser.buffer_posn < parser->tokeniser.buffer_len) {
            /* If "COPY TAPE" is followed by a comma, then suppress the
             * output of "~~~~~" blanks at runtime.  We also do this in the
             * extended syntax as blanks are useless. */
            if (parser->tokeniser.buffer[parser->tokeniser.buffer_posn] == ',') {
                ++(parser->tokeniser.buffer_posn);
                token = ITOK_COPY_NO_BLANKS;
            } else if ((parser->flags & ITOK_TYPE_EXTENSION) != 0) {
                token = ITOK_COPY_NO_BLANKS;
            }
        }
        node = ip_ast_make_standalone(token, &(parser->tokeniser.loc));
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        break;

    case ITOK_IGNORE_TAPE:
        node = ip_ast_make_standalone(token, &(parser->tokeniser.loc));
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        break;

    /* ------------- String statements ------------- */

    case ITOK_SUBSTRING:
        node = ip_parse_extract_substring(parser);
        break;

    case ITOK_LENGTH_OF:
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        node = ip_ast_make_this_unary
            (token, parser->this_type, IP_TYPE_STRING,
             &(parser->tokeniser.loc));
        node->value_type = IP_TYPE_INT;
        node->this_type = IP_TYPE_INT;
        break;

    default:
        /* No idea what this is, so report an error for the current token */
        ip_error_near(parser, 0);
        break;
    }
    if (node && node->this_type != IP_TYPE_UNKNOWN) {
        parser->this_type = node->this_type;
    }
    if (node) {
        /* Remember the statement type for '&' repetition */
        parser->last_statement = node->type;
    }
    return node;
}

/*
 * Label ::= ( "*" INTEGER | "*" VAR-NAME )
 */
static void ip_parse_statement_label(ip_parser_t *parser)
{
    ip_label_t *label = 0;
    ip_ast_node_t *stmt;
    const char *name;

    /* Skip the "*" that marks the label */
    ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);

    /* Do we have a numeric label or a named label? */
    if (parser->tokeniser.token == ITOK_INT_VALUE) {
        /* Label that is referred to by number */
        ip_int_t num = (ip_int_t)(parser->tokeniser.ivalue);
        if (num < IP_MIN_LABEL_NUMBER || num > IP_MAX_LABEL_NUMBER) {
            ip_error(parser, "label numbers must be between %u and %u",
                     (unsigned)IP_MIN_LABEL_NUMBER,
                     (unsigned)IP_MAX_LABEL_NUMBER);
            ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
            return;
        }
        label = ip_label_lookup_by_number(&(parser->program->labels), num);
        if (label) {
            if (ip_label_is_defined(label)) {
                ip_error(parser, "label %u is already defined", (unsigned)num);
                ip_error_at(parser, &(label->node->loc),
                            "previous definition here");
                label = 0;
            } else {
                /* Forward reference to this label that is now defined */
                ip_label_mark_as_defined(label);
            }
        } else {
            label = ip_label_create_by_number(&(parser->program->labels), num);
            ip_label_mark_as_defined(label);
        }
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
    } else if ((parser->tokeniser.token == ITOK_VAR_NAME ||
                parser->tokeniser.token == ITOK_ROUTINE_NAME) &&
               (parser->flags & ITOK_TYPE_EXTENSION) != 0) {
        /* Label that is referred to by name */
        name = parser->tokeniser.token_info->name;
        label = ip_label_lookup_by_name(&(parser->program->labels), name);
        if (label) {
            if (ip_label_is_defined(label)) {
                ip_error(parser, "label '%s' is already defined", name);
                ip_error_at(parser, &(label->node->loc),
                            "previous definition here");
                label = 0;
            } else {
                /* Forward reference to this label that is now defined */
                ip_label_mark_as_defined(label);
            }
        } else {
            label = ip_label_create_by_name(&(parser->program->labels), name);
            ip_label_mark_as_defined(label);
        }
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
    } else if (parser->tokeniser.token == ITOK_STR_VALUE &&
               (parser->flags & ITOK_TYPE_EXTENSION) != 0) {
        /* Named label using a string */
        if (ip_parse_verify_label_string(parser)) {
            name = parser->tokeniser.token_info->name;
            label = ip_label_lookup_by_name(&(parser->program->labels), name);
            if (label) {
                if (ip_label_is_defined(label)) {
                    ip_error(parser, "label '%s' is already defined", name);
                    ip_error_at(parser, &(label->node->loc),
                                "previous definition here");
                    label = 0;
                } else {
                    /* Forward reference to this label that is now defined */
                    ip_label_mark_as_defined(label);
                }
            } else {
                label = ip_label_create_by_name
                    (&(parser->program->labels), name);
                ip_label_mark_as_defined(label);
            }
        } else {
            ip_error(parser, "invalid label string");
        }
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
    } else if ((parser->flags & ITOK_TYPE_EXTENSION) != 0) {
        ip_error(parser, "label number or name expected");
    } else {
        ip_error(parser, "label number expected");
    }

    /* Construct a statement node and link it to the label */
    if (label) {
        stmt = ip_ast_make_standalone(ITOK_LABEL, &(parser->tokeniser.loc));
        stmt->label = label;
        stmt->this_type = IP_TYPE_DYNAMIC;
        label->node = stmt;
        ip_ast_list_add(&(parser->program->statements), stmt);
    }
}

/*
 * Statements ::= { StatementLine } EOF
 * StatementLine ::= [ Labels Statement { "," Labels Statement } ] [ "," ] EOL
 * Labels ::= { Label }
 */
void ip_parse_statements(ip_parser_t *parser)
{
    ip_ast_node_t *stmt;
    while (parser->tokeniser.token != ITOK_EOF &&
           parser->tokeniser.token != ITOK_INPUT_DATA) {
        while (!ip_parse_token_is_terminator(parser->tokeniser.token)) {

            /* Handle "GO TO" labels within the line.  Technically they
             * should only be allowed at the start of the line, but we
             * allow them before any statement for simplicity. */
            while (parser->tokeniser.token == ITOK_LABEL) {
                /* Parse the label number or name */
                ip_parse_statement_label(parser);

                /* We reset the type of "THIS" to DYNAMIC when we
                 * reach a label because don't know what it might
                 * be on entry to the label from a "GO TO". */
                parser->this_type = IP_TYPE_DYNAMIC;
            }

            /* Parse the next statement and add it to the list */
            stmt = ip_parse_statement(parser);
            ip_ast_list_add(&(parser->program->statements), stmt);

            /* Deal with comma separators between the statements,
             * or error out if we have a massive parse failure. */
            if (parser->tokeniser.token == ITOK_COMMA) {
                if (stmt && stmt->type == ITOK_OUTPUT) {
                    /* "OUTPUT X," indicates that the output should
                     * not result in an end of line sequence. */
                    stmt->type = ITOK_OUTPUT_NO_EOL;
                }
                ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
            } else if (!ip_parse_token_is_terminator(parser->tokeniser.token)) {
                /* We don't know what this is.  Print an error and
                 * skip tokens until we see end of line. */
                ip_error_near(parser, 0);
                while (!ip_parse_token_is_terminator(parser->tokeniser.token)) {
                    ip_parse_get_next(parser, ITOK_TYPE_ANY);
                }
            }
        }
        if (parser->tokeniser.token == ITOK_EOL) {
            /* Add an EOL marker to the code so that "IF" statements
             * know where to skip forward to for the else condition. */
            stmt = ip_ast_make_text
                (ITOK_EOL, parser->tokeniser.token_info->name,
                 &(parser->tokeniser.loc));
            ip_ast_list_add(&(parser->program->statements), stmt);
            ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);

            /* "&" stops repeating when the end of the line is reached */
            parser->last_statement = -1;
        }
    }
    if (parser->tokeniser.token == ITOK_INPUT_DATA) {
        /* Record the embedded input data at the end of the program */
        ip_program_set_input
            (parser->program, parser->tokeniser.token_info->name);
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
    }
}

/**
 * @brief Parses symbol definitions in preliminary statement (2).
 *
 * @param[in,out] parser The parsing context.
 */
static void ip_parse_symbols(ip_parser_t *parser)
{
    const char *name;
    ip_var_t *var;
    unsigned char type = IP_TYPE_INT;

    /* Bail out if we don't have the correct prefix */
    if (parser->tokeniser.token == ITOK_SYMBOLS_STR) {
        /* "SYMBOLS FOR STRINGS" keyword */
        type = IP_TYPE_STRING;
    } else if (parser->tokeniser.token == ITOK_SYMBOLS_ROUTINES) {
        /* "SYMBOLS FOR ROUTINES" keyword */
        type = IP_TYPE_ROUTINE;
    } else if (parser->tokeniser.token != ITOK_SYMBOLS_INT) {
        ip_error(parser, "'SYMBOLS FOR INTEGERS' expected");
        return;
    }

    /* Skip the prefix and check for "NONE" */
    ip_parse_get_next(parser, ITOK_TYPE_PRELIM_2);
    if (parser->tokeniser.token == ITOK_NONE) {
        ip_parse_get_next(parser, ITOK_TYPE_PRELIM_2);
        if (!ip_parse_token_is_terminator(parser->tokeniser.token)) {
            ip_error(parser, "end of line expected");
        }
        return;
    }

    /* Parse a comma-separated list of symbols (commas are optional) */
    while (!ip_parse_token_is_terminator(parser->tokeniser.token)) {
        if (parser->tokeniser.token == ITOK_COMMA) {
            /* Skip the comma */
            ip_parse_get_next(parser, ITOK_TYPE_PRELIM_2);
        } else if (parser->tokeniser.token == ITOK_VAR_NAME ||
                   (parser->tokeniser.token == ITOK_STR_VALUE &&
                    type == IP_TYPE_ROUTINE &&
                    ip_parse_verify_label_string(parser))) {
            /* Look up the name and create a new variable if necessary */
            name = parser->tokeniser.token_info->name;
            var = ip_var_lookup(&(parser->program->vars), name);
            if (var) {
                ip_error(parser, "symbol '%s' is already declared", name);
            } else {
                ip_var_create(&(parser->program->vars), name, type);
            }
            if (type == IP_TYPE_ROUTINE) {
                /* Also create a routine label with the same name */
                ip_label_t *label = ip_label_lookup_by_name
                    (&(parser->program->labels), name);
                if (!label) {
                    label = ip_label_create_by_name
                        (&(parser->program->labels), name);
                }
                label->base.type = IP_TYPE_ROUTINE;
                ip_tokeniser_register_routine_name(&(parser->tokeniser), name);
            }
            ip_parse_get_next(parser, ITOK_TYPE_PRELIM_2);
        } else {
            ip_error_near(parser, "symbol name expected");
            break;
        }
    }
}

/*
 * ArraySize ::= [ "-" ] INTEGER
 */
static int ip_parse_array_size
    (ip_parser_t *parser, ip_int_t *size, int min_subscript)
{
    int is_neg = 0;
    *size = 0;
    if (min_subscript && parser->tokeniser.token != ITOK_LPAREN) {
        ip_error_near(parser, "'(' expected");
        return 0;
    }
    ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
    if (parser->tokeniser.token == ITOK_MINUS) {
        is_neg = 1;
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
    } else if (parser->tokeniser.token == ITOK_PLUS) {
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
    }
    if (parser->tokeniser.token == ITOK_INT_VALUE) {
        *size = (ip_int_t)(parser->tokeniser.ivalue);
        if (is_neg) {
            *size = -(*size);
        }
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        if (parser->tokeniser.token == ITOK_RPAREN) {
            ip_parse_get_next(parser, ITOK_TYPE_PRELIM_3);
            return 1;
        } else if (min_subscript && parser->tokeniser.token == ITOK_COLON) {
            return 2;
        }
        if (min_subscript) {
            ip_error_near(parser, "')' or ':' expected");
        } else {
            ip_error_near(parser, "')' expected");
        }
    } else {
        ip_error_near(parser, "integer constant expected");
    }
    return 0;
}

/**
 * @brief Parses array definitions in preliminary statement (3).
 *
 * @param[in,out] parser The parsing context.
 */
static void ip_parse_arrays(ip_parser_t *parser)
{
    const char *name;
    ip_var_t *var;
    ip_int_t size;
    ip_int_t size2;
    int status;

    /* Bail out if we don't have the correct prefix */
    if (parser->tokeniser.token != ITOK_MAX_SUBSCRIPTS) {
        ip_error(parser, "'MAXIMUM SUBSCRIPTS' expected");
        return;
    }

    /* Skip the prefix */
    ip_parse_get_next(parser, ITOK_TYPE_PRELIM_3);

    /* Parse a comma-separated list of array definitions */
    while (!ip_parse_token_is_terminator(parser->tokeniser.token)) {
        if (parser->tokeniser.token == ITOK_COMMA) {
            /* Skip the comma */
            ip_parse_get_next(parser, ITOK_TYPE_PRELIM_3);
        } else if (parser->tokeniser.token == ITOK_VAR_NAME) {
            /* Look up the name and create a new variable if necessary */
            name = parser->tokeniser.token_info->name;
            var = ip_var_lookup(&(parser->program->vars), name);
            if (!var) {
                var = ip_var_create
                    (&(parser->program->vars), name, IP_TYPE_FLOAT);
            }
            if (ip_var_is_array(var)) {
                ip_error
                    (parser,
                     "symbol '%s' is already declared as an array",
                     name);
            }
            ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
            status = ip_parse_array_size(parser, &size, 1);
            if (status == 1) {
                /* Dimensions are specified as A(N) or A(-N) */
                if (size < 0) {
                    ip_var_dimension_array(var, size, 0);
                } else {
                    ip_var_dimension_array(var, 0, size);
                }
            } else if (status == 2) {
                /* Dimensions are specified as A(min:max) */
                if (ip_parse_array_size(parser, &size2, 0)) {
                    if (size > size2) {
                        ip_int_t temp = size;
                        size = size2;
                        size2 = temp;
                        ip_warning(parser, "minimum subscript is greater than maximum");
                    }
                    ip_var_dimension_array(var, size, size2);
                } else {
                    break;
                }
            } else {
                break;
            }
        } else {
            ip_error_near(parser, "symbol name expected");
            break;
        }
    }
}

/*
 * Classic INTERPROGRAM requires (1), (2), and (4), with (3) optional.
 * Classic INTERPROGRAM limits the number of integer variables to 8,
 * which we don't do here.  It also allows (3) to be repeated multiple times.
 *
 * Extended INTERPROGRAM allows everything to be optional but they must
 * be in the order (1), (2), (3), (4) when they are provided, with
 * repetition of (2) allowed in addition to (3).
 *
 * ClassicPreliminaryStatements ::=
 *      Title SymbolDeclarations { ArrayDeclarations } Compile
 *
 * ExtendedPreliminaryStatements ::=
 *      [ Title ] { SymbolDeclarations } { ArrayDeclarations } [ Compile ]
 *
 * Title ::=
 *      "(1)" "TITLE" TEXT EOL
 *
 * SymbolDeclarations ::=
 *      "(2)" "SYMBOLS FOR INTEGERS" Symbols EOL
 * Symbols ::=
 *      VAR-NAME { [ "," ] VAR-NAME }
 *    | "NONE"
 *
 * ArrayDeclarations ::=
 *      "(3)" "MAXIMUM SUBSCRIPTS" Arrays EOL
 * Arrays ::=
 *      ArrayDeclaration { [ "," ] ArrayDeclaration }
 * ArrayDeclaration ::=
 *      VAR-NAME "(" INTEGER ")"
 *    | VAR-NAME "(" "-" INTEGER ")"
 *
 * Compile ::=
 *      "(4)" "COMPILE THE FOLLOWING INTERPROGRAM" EOL
 */
void ip_parse_preliminary_statements(ip_parser_t *parser)
{
    ip_ast_node_t *stmt;
    char *title;
    int token;
    int sections = 0;

    /* Parse the numbered preliminary statements */
    ip_parse_get_next(parser, ITOK_TYPE_PRELIM_START);
    token = parser->tokeniser.token;
    while ((token >= ITOK_PRELIM_1 && token <= ITOK_PRELIM_4) ||
                token == ITOK_EOL) {
        /* Skip the (n) token */
        ip_parse_get_next(parser, ITOK_TYPE_PRELIM_START);
        if (token == ITOK_EOL) {
            token = parser->tokeniser.token;
            continue;
        }

        /* Determine what kind of preliminary statement we have */
        switch (token) {
        case ITOK_PRELIM_1:
            /* Title for the program */
            if ((sections & 0x0001) != 0) {
                ip_error(parser, "multiple title statements (1)");
                break;
            }
            if ((sections & 0xFFFE) != 0) {
                ip_error(parser, "preliminary statement (1) is out of order");
            }
            if (parser->tokeniser.token != ITOK_TITLE) {
                ip_error(parser, "'TITLE' expected");
            }
            title = ip_tokeniser_read_title(&(parser->tokeniser));
            stmt = ip_ast_make_text
                (ITOK_TITLE, title, &(parser->tokeniser.loc));
            ip_ast_list_add(&(parser->program->statements), stmt);
            sections |= 0x0001;
            break;

        case ITOK_PRELIM_2:
            /* Declaration of non floating-point symbols */
            if ((sections & 0x0002) != 0 &&
                    (parser->flags & ITOK_TYPE_EXTENSION) == 0) {
                /* Classic INTERPROGRAM only allows one (2) statement */
                ip_error(parser, "multiple symbol declaration statements (2)");
            }
            if ((sections & 0xFFFC) != 0) {
                ip_error(parser, "preliminary statement (2) is out of order");
            }
            ip_parse_symbols(parser);
            sections |= 0x0002;
            break;

        case ITOK_PRELIM_3:
            /* Declaration of array variables */
            if ((sections & 0xFFF8) != 0) {
                ip_error(parser, "preliminary statement (3) is out of order");
            }
            ip_parse_arrays(parser);
            sections |= 0x0004;
            break;

        case ITOK_PRELIM_4:
            /* End of the preliminary statements */
            if (parser->tokeniser.token != ITOK_COMPILE_PROGRAM) {
                ip_error
                    (parser, "'COMPILE THE FOLLOWING INTERPROGRAM' expected");
            }
            stmt = ip_ast_make_standalone
                (ITOK_COMPILE_PROGRAM, &(parser->tokeniser.loc));
            ip_ast_list_add(&(parser->program->statements), stmt);
            sections |= 0x0008;
            break;
        }

        /* Skip tokens until the next end of line */
        ip_tokeniser_skip_line(&(parser->tokeniser));

        /* If we have seen "COMPILE THE FOLLOWING INTERPROGRAM", then stop */
        if ((sections & 0x0008) != 0) {
            ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
            break;
        }

        /* Read the start of the preliminary statement on the next line */
        ip_parse_get_next(parser, ITOK_TYPE_PRELIM_START);
        token = parser->tokeniser.token;
    }

    /* Check for missing statements in the classic syntax */
    if ((parser->flags & ITOK_TYPE_EXTENSION) == 0) {
        /* We must have at least (1), (2), and (4) */
        if ((sections & 0x0001) == 0) {
            ip_error(parser, "missing title statement (1)");
        }
        if ((sections & 0x0002) == 0) {
            ip_error(parser, "missing symbol declaration statement (2)");
        }
        if ((sections & 0x0008) == 0) {
            ip_error(parser, "missing compilation statement (4)");
        }
    }
}

static void ip_parse_check_label(ip_label_t *label, void *user_data)
{
    ip_parser_t *parser = (ip_parser_t *)user_data;
    if (!ip_label_is_defined(label)) {
        if (ip_label_get_name(label)) {
            ip_error(parser, "undefined label '%s'",
                     ip_label_get_name(label));
        } else {
            ip_error(parser, "undefined label %u",
                     (unsigned)(ip_label_get_number(label)));
        }
    }
}

void ip_parse_check_undefined_labels(ip_parser_t *parser)
{
    ip_label_table_visit
        (&(parser->program->labels), ip_parse_check_label, parser);
}

void ip_parse_check_open_blocks(ip_parser_t *parser)
{
    while (parser->blocks) {
        ip_ast_node_t *node = parser->blocks->control;
        if (parser->blocks->type == ITOK_IF) {
            ip_error_at(parser, &(node->loc), "unterminated 'IF'");
        } else {
            ip_error_at(parser, &(node->loc), "unterminated 'REPEAT WHILE'");
        }
        ip_parse_free_top_block(parser);
    }
}

static int ip_parse_read_stdio(FILE *input)
{
    return getc(input);
}

unsigned long ip_parse_program_file
    (ip_program_t **program, const char *filename, unsigned options,
     int argc, char **argv)
{
    FILE *input;
    ip_parser_t parser;
    unsigned long num_errors;
    int index;

    /* Construct the program image */
    *program = ip_program_new(filename ? filename : "-");

    /* Create the "ARGV" variable if necessary */
    if (argc > 0) {
        ip_var_t *var = ip_var_create
            (&((*program)->vars), "ARGV", IP_TYPE_STRING);
        ip_var_dimension_array(var, 0, argc - 1);
        var->base.flags |= IP_SYMBOL_NO_RESET;
        for (index = 0; index < argc; ++index) {
            ip_string_t *str = ip_string_create(argv[index]);
            ip_value_t value;
            value.type = IP_TYPE_STRING;
            value.svalue = str;
            ip_value_to_array(var, index, &value);
            ip_value_release(&value);
        }
    }

    /* Open the input file */
    if (filename) {
        input = fopen(filename, "r");
        if (!input) {
            perror(filename);
            return 1; /* One error occurred */
        }
    } else {
        input = stdin;
    }

    /* Initialise the parser and tokeniser */
    ip_parse_init(&parser);
    parser.flags = options;
    parser.tokeniser.read_char = (ip_token_read_char)ip_parse_read_stdio;
    parser.tokeniser.user_data = input;
    parser.program = *program;
    if (filename) {
        /* Use the permanent version of the filename for setting the
         * locations of nodes in the abstract syntax tree. */
        parser.tokeniser.filename = (*program)->filename;
    }

    /* Parse the contents of the program file */
    ip_parse_preliminary_statements(&parser);
    ip_parse_statements(&parser);
    ip_parse_check_undefined_labels(&parser);
    ip_parse_check_open_blocks(&parser);

    /* Clean up and exit */
    if (filename) {
        fclose(input);
    }
    num_errors = parser.num_errors;
    ip_parse_free(&parser);
    return num_errors;
}
