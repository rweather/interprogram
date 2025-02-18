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

void ip_parse_free(ip_parser_t *parser)
{
    ip_tokeniser_free(&(parser->tokeniser));
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

/*
 * Variable ::= VAR-NAME [ '(' Expression ')' ]
 */
static ip_ast_node_t *ip_parse_variable_expression
    (ip_parser_t *parser, int allowed)
{
    ip_ast_node_t *node = 0;
    ip_var_t *var;

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
    switch (var->type) {
    case IP_TYPE_INT:
    case IP_TYPE_FLOAT:
        node = ip_ast_make_variable(var, &(parser->tokeniser.loc));
        if ((allowed & IP_VAR_ALLOW_ARRAYS) != 0 &&
                parser->tokeniser.token == ITOK_LPAREN) {
            /* We cannot use an index with this variable, but parse it anyway */
            ip_error(parser, "variable '%s' is not an array", var->name);
            ip_ast_node_free(ip_parse_extended_expression(parser));
        }
        break;

    case IP_TYPE_ARRAY_OF_INT:
    case IP_TYPE_ARRAY_OF_FLOAT:
        if (parser->tokeniser.token == ITOK_LPAREN) {
            /* Parse the array index expression */
            node = ip_parse_extended_expression(parser);

            /* Construct a lookup of the specific array index */
            node = ip_ast_make_array_access
                (var, node, &(parser->tokeniser.loc));
        } else if ((allowed & IP_VAR_ALLOW_ARRAYS) == 0) {
            ip_error_near
                (parser, "array variable '%s' is not permitted here",
                 var->name);
        } else {
            ip_error_near(parser, "array index expected", var->name);
        }
        break;

    default:
        ip_error(parser, "variable '%s' is not permitted here", var->name);
        break;
    }
    return node;
}

/*
 * UnaryExpression ::= {"+" | "-"} BaseExpression
 * BaseExpression ::=
 *      "THIS"
 *    | NUMBER
 *    | Variable
 *    | "(" Expression ")"
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
        /* Parse the variable name, optionally followed by an array index */
        node = ip_parse_variable_expression(parser, IP_VAR_ALLOW_ARRAYS);
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

    default:
        ip_error_near(parser, "variable name or number expected");
        node = 0;
        break;
    }

    /* Negate the base expression by subtracting from zero if necessary */
    if (is_neg && node) {
        node = ip_parse_negate_node(parser, node);
    }
    return node;
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
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        node2 = ip_parse_unary_expression(parser);
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
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        node2 = ip_parse_multiplicative_expression(parser);
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
        node = ip_parse_variable_expression(parser, 0);
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
            node = ip_parse_variable_expression(parser, 0);
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

    /* Parse the first expression in the conditional */
    node = ip_parse_next_expression(parser);

    /* Determine what kind of condition we have */
    token = parser->tokeniser.token;
    switch (token) {
    case ITOK_GREATER_THAN:
    case ITOK_MUCH_GREATER_THAN:
    case ITOK_SMALLER_THAN:
    case ITOK_MUCH_SMALLER_THAN:
    case ITOK_EQUAL_TO:
    case ITOK_NOT_EQUAL_TO:
        node2 = ip_parse_next_expression(parser);
        node = ip_ast_make_binary
            (token, node, node2, &(parser->tokeniser.loc));
        if (node) {
            /* Conditions always have a boolean result */
            node->value_type = IP_TYPE_INT;
        }
        break;

    case ITOK_ZERO:
    case ITOK_NOT_ZERO:
    case ITOK_POSITIVE:
    case ITOK_NEGATIVE:
    case ITOK_FINITE:
    case ITOK_INFINITE:
    case ITOK_NAN:
        node = ip_ast_make_unary(token, node, &(parser->tokeniser.loc));
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
 * IfStatement ::= "IF" Condition
 */
static ip_ast_node_t *ip_parse_if_statement(ip_parser_t *parser)
{
    ip_ast_node_t *node = ip_parse_condition(parser);
    return ip_ast_make_unary_statement
        (ITOK_IF, IP_TYPE_UNKNOWN, node, &(parser->tokeniser.loc));
}

/*
 * LabelName ::=
 *      "*" Expression
 *    | VAR-NAME                        # Extension
 */
static ip_ast_node_t *ip_parse_label_name(ip_parser_t *parser)
{
    ip_ast_node_t *node = 0;
    ip_label_t *label = 0;
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
    } else if (parser->tokeniser.token == ITOK_VAR_NAME &&
               (parser->flags & ITOK_TYPE_EXTENSION) != 0) {
        /* Named label */
        const char *name = parser->tokeniser.token_info->name;
        label = ip_label_lookup_by_name(&(parser->program->labels), name);
        if (!label) {
            label = ip_label_create_by_name
                (&(parser->program->labels), name);
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
 * Statement ::=
 *      AssignmentStatement
 *    | ArithmeticStatement
 *    | BitwiseStatement
 *    | MathFunctionStatement
 *    | IfStatement
 *    | ControlFlowStatement
 *    | InputOutputStatement
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
 *
 * ControlFlowStatement ::=
 *      "GO TO" LabelName
 *    | "EXECUTE PROCESS" LabelName
 *    | "REPEAT FROM" LabelName VAR-NAME "TIMES"
 *    | "END OF PROCESS DEFINITION"
 *    | "END OF INTERPROGRAM"
 *    | "PAUSE"
 *    | "CALL" LabelName                # Extensions
 *    | "RETURN"
 *    | "RETURN" Expression
 *
 * InputOutputStatement ::=
 *      "INPUT" Variable
 *    | "INPUT"
 *    | "OUTPUT" Expression
 *    | "OUTPUT"
 *    | "PUNCH THE FOLLOWING CHARACTERS" EOL TEXT BLANKS
 *    | "COPY TAPE"
 *    | "IGNORE TAPE"
 */
static ip_ast_node_t *ip_parse_statement(ip_parser_t *parser)
{
    int token = parser->tokeniser.token;
    ip_ast_node_t *node = 0;
    ip_ast_node_t *var;
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
        var = ip_parse_variable_expression(parser, IP_VAR_ALLOW_ARRAYS);
        node = ip_ast_make_unary_statement
            (ITOK_REPLACE, IP_TYPE_UNKNOWN, var, &(parser->tokeniser.loc));
        break;

    case ITOK_SET:
        /* SET variable = expression */
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        if ((parser->flags & ITOK_TYPE_EXTENSION) != 0) {
            /* Extended INTERPROGRAM allows assigning array elements */
            var = ip_parse_variable_expression(parser, IP_VAR_ALLOW_ARRAYS);
        } else {
            /* Classic INTERPROGRAM only allows simple variables for "SET" */
            var = ip_parse_variable_expression(parser, 0);
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
    case ITOK_LOG:
    case ITOK_EXP:
    case ITOK_ABS:
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

    /* ------------- Conditional statements ------------- */

    case ITOK_IF:
        node = ip_parse_if_statement(parser);
        break;

    /* ------------- Control flow statements ------------- */

    case ITOK_GO_TO:
    case ITOK_EXECUTE_PROCESS:
    case ITOK_CALL:
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        node = ip_parse_label_name(parser);
        node = ip_ast_make_unary_statement
            (token, IP_TYPE_DYNAMIC, node, &(parser->tokeniser.loc));
        break;

    case ITOK_REPEAT_FROM:
        /* REPEAT FROM *expression variable TIMES */
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        if (parser->tokeniser.token == ITOK_LABEL) {
            node = ip_parse_next_expression(parser);
            var = ip_parse_variable_expression(parser, 0);
            if (var && var->type != IP_TYPE_INT) {
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
                    if (parser->tokeniser.token != ITOK_EOL &&
                            parser->tokeniser.token != ITOK_EOF) {
                        ip_error(parser, "end of line expected after 'REPEAT FROM' statement");
                    }
                }
            } else {
                ip_error_near(parser, "'TIMES' expected");
            }
        } else {
            ip_error_near(parser, "label reference expected");
        }
        break;

    case ITOK_END_PROCESS:
    case ITOK_END_PROGRAM:
    case ITOK_PAUSE:
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
                parser->tokeniser.token == ITOK_EOL ||
                parser->tokeniser.token == ITOK_EOF) {
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

    /* ------------- Input/Output statements ------------- */

    case ITOK_INPUT:
        /*
         * INPUT variable
         * INPUT
         */
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        if (parser->tokeniser.token == ITOK_VAR_NAME) {
            /* Input into the variable and "THIS" */
            var = ip_parse_variable_expression(parser, IP_VAR_ALLOW_ARRAYS);
        } else {
            /* Input into "THIS" only; always floating-point */
            var = ip_ast_make_this(IP_TYPE_FLOAT, &(parser->tokeniser.loc));
            ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        }
        if (var) {
            /* The type of "THIS" is set to that of the variable because the
             * "INPUT" statement also side-effects "THIS". */
            node = ip_ast_make_unary_statement
                (ITOK_INPUT, var->value_type, var, &(parser->tokeniser.loc));
        }
        break;

    case ITOK_OUTPUT:
        /*
         * OUTPUT expression
         * OUTPUT
         */
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        if (parser->tokeniser.token == ITOK_COMMA ||
                parser->tokeniser.token == ITOK_EOL ||
                parser->tokeniser.token == ITOK_EOF) {
            /* Output "THIS" with default formatting */
            node = ip_ast_make_standalone
                (ITOK_OUTPUT, &(parser->tokeniser.loc));
        } else {
            /* Output the value of an expression with complex formatting */
            node = ip_parse_expression(parser);
            if (node) {
                node = ip_ast_make_unary_statement
                    (ITOK_OUTPUT, IP_TYPE_UNKNOWN, node,
                     &(parser->tokeniser.loc));
            }
        }
        break;

    case ITOK_PUNCH:
        text = ip_tokeniser_read_punch(&(parser->tokeniser));
        node = ip_ast_make_text(ITOK_PUNCH, text, &(parser->tokeniser.loc));
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
        break;

    case ITOK_COPY_TAPE:
    case ITOK_IGNORE_TAPE:
        node = ip_ast_make_standalone(token, &(parser->tokeniser.loc));
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
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
            if (label->is_defined) {
                ip_error(parser, "label %u is already defined", (unsigned)num);
                ip_error_at(parser, &(label->node->loc),
                            "previous definition here");
                label = 0;
            } else {
                /* Forward reference to this label that is now defined */
                label->is_defined = 1;
            }
        } else {
            label = ip_label_create_by_number(&(parser->program->labels), num);
            label->is_defined = 1;
        }
        ip_parse_get_next(parser, ITOK_TYPE_STATEMENT);
    } else if (parser->tokeniser.token == ITOK_VAR_NAME &&
               (parser->flags & ITOK_TYPE_EXTENSION) != 0) {
        /* Label that is referred to by name */
        const char *name = parser->tokeniser.token_info->name;
        label = ip_label_lookup_by_name(&(parser->program->labels), name);
        if (label) {
            if (label->is_defined) {
                ip_error(parser, "label '%s' is already defined", name);
                ip_error_at(parser, &(label->node->loc),
                            "previous definition here");
                label = 0;
            } else {
                /* Forward reference to this label that is now defined */
                label->is_defined = 1;
            }
        } else {
            label = ip_label_create_by_name(&(parser->program->labels), name);
            label->is_defined = 1;
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
    while (parser->tokeniser.token != ITOK_EOF) {
        while (parser->tokeniser.token != ITOK_EOL &&
               parser->tokeniser.token != ITOK_EOF) {

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
            } else if (parser->tokeniser.token != ITOK_EOL &&
                       parser->tokeniser.token != ITOK_EOF) {
                /* We don't know what this is.  Print an error and
                 * skip tokens until we see end of line. */
                ip_error_near(parser, 0);
                while (parser->tokeniser.token != ITOK_EOL &&
                       parser->tokeniser.token != ITOK_EOF) {
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
    if (parser->tokeniser.token != ITOK_SYMBOLS_INT) {
        ip_error(parser, "'SYMBOLS FOR INTEGERS' expected");
        return;
    }

    /* Skip the prefix and check for "NONE" */
    ip_parse_get_next(parser, ITOK_TYPE_PRELIM_2);
    if (parser->tokeniser.token == ITOK_NONE) {
        ip_parse_get_next(parser, ITOK_TYPE_PRELIM_2);
        if (parser->tokeniser.token != ITOK_EOL &&
                parser->tokeniser.token != ITOK_EOF) {
            ip_error(parser, "end of line expected");
        }
        return;
    }

    /* Parse a comma-separated list of symbols (commas are optional) */
    while (parser->tokeniser.token != ITOK_EOL &&
           parser->tokeniser.token != ITOK_EOF) {
        if (parser->tokeniser.token == ITOK_COMMA) {
            /* Skip the comma */
            ip_parse_get_next(parser, ITOK_TYPE_PRELIM_2);
        } else if (parser->tokeniser.token == ITOK_VAR_NAME) {
            /* Look up the name and create a new variable if necessary */
            name = parser->tokeniser.token_info->name;
            var = ip_var_lookup(&(parser->program->vars), name);
            if (var) {
                ip_error(parser, "symbol '%s' is already declared", name);
            } else {
                ip_var_create(&(parser->program->vars), name, type);
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
static int ip_parse_array_size(ip_parser_t *parser, ip_int_t *size)
{
    int is_neg = 0;
    *size = 0;
    if (parser->tokeniser.token == ITOK_LPAREN) {
        ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
        if (parser->tokeniser.token == ITOK_MINUS) {
            is_neg = 1;
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
            }
            ip_error_near(parser, "')' expected");
        } else {
            ip_error_near(parser, "integer constant expected");
        }
    } else {
        ip_error_near(parser, "'(' expected");
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

    /* Bail out if we don't have the correct prefix */
    if (parser->tokeniser.token != ITOK_MAX_SUBSCRIPTS) {
        ip_error(parser, "'MAXIMUM SUBSCRIPTS' expected");
        return;
    }

    /* Skip the prefix */
    ip_parse_get_next(parser, ITOK_TYPE_PRELIM_3);

    /* Parse a comma-separated list of array definitions */
    while (parser->tokeniser.token != ITOK_EOL &&
           parser->tokeniser.token != ITOK_EOF) {
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
            if (var->type == IP_TYPE_ARRAY_OF_INT ||
                    var->type == IP_TYPE_ARRAY_OF_FLOAT) {
                ip_error
                    (parser,
                     "symbol '%s' is already declared as an array",
                     name);
            }
            ip_parse_get_next(parser, ITOK_TYPE_EXPRESSION);
            if (ip_parse_array_size(parser, &size)) {
                ip_var_dimension_array(var, size);
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

static void ip_parse_check_labels(ip_parser_t *parser, ip_label_t *label)
{
    /* Walk the tree in alphanumeric order and print the undefined labels */
    if (label != &(parser->program->labels.nil)) {
        ip_parse_check_labels(parser, label->left);
        if (!(label->is_defined)) {
            if (label->name) {
                ip_error(parser, "undefined label '%s'", label->name);
            } else {
                ip_error(parser, "undefined label %u", (unsigned)(label->num));
            }
        }
        ip_parse_check_labels(parser, label->right);
    }
}

void ip_parse_check_undefined_labels(ip_parser_t *parser)
{
    ip_parse_check_labels(parser, parser->program->labels.root.right);
}

static int ip_parse_read_stdio(FILE *input)
{
    return getc(input);
}

unsigned long ip_parse_program_file
    (ip_program_t **program, const char *filename, unsigned options)
{
    FILE *input;
    ip_parser_t parser;
    unsigned long num_errors;

    /* Construct the program image */
    *program = ip_program_new(filename ? filename : "-");

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

    /* Clean up and exit */
    if (filename) {
        fclose(input);
    }
    num_errors = parser.num_errors;
    ip_parse_free(&parser);
    return num_errors;
}
