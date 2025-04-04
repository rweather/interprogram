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

#include "ip_exec.h"
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

void ip_exec_init(ip_exec_t *exec, ip_program_t *program)
{
    memset(exec, 0, sizeof(ip_exec_t));
    exec->program = program;
    ip_value_init(&(exec->this_value));
    exec->pc = exec->program->statements.first;
    ip_program_reset_variables(exec->program);
    exec->input = stdin;
    exec->output = stdout;
    srand(time(0));
}

/**
 * @brief Frees an item on the execution stack.
 *
 * @param[in] item The item to be freed.
 */
static void ip_exec_free_stack_item(ip_exec_stack_item_t *item)
{
    unsigned index;
    if (item->type == ITOK_CALL) {
        ip_exec_stack_call_t *frame = (ip_exec_stack_call_t *)item;
        for (index = 0; index < IP_MAX_LOCALS; ++index) {
            ip_value_release(&(frame->locals[index]));
        }
    } else {
        ip_exec_stack_loop_t *loop = (ip_exec_stack_loop_t *)item;
        ip_value_release(&(loop->end));
        ip_value_release(&(loop->step));
    }
    free(item);
}

/**
 * @brief Pops the execution stack until a specific item is reached,
 * or all items have been popped.
 *
 * @param[in] exec The execution context.
 * @param[in] to The item to stop popping at.
 */
static void ip_exec_pop_stack_to(ip_exec_t *exec, ip_exec_stack_item_t *to)
{
    while (exec->stack && exec->stack != to) {
        ip_exec_stack_item_t *next = exec->stack->next;
        ip_exec_free_stack_item(exec->stack);
        exec->stack = next;
    }
}

/**
 * @brief Find the execution stack item for the current subroutine call.
 *
 * @param[in] exec The execution context.
 *
 * @return A pointer to the call's execution stack item, or NULL if
 * we are at the top-most level of the program.
 */
static ip_exec_stack_call_t *ip_exec_find_call(ip_exec_t *exec)
{
    ip_exec_stack_item_t *item = exec->stack;
    while (item && item->type != ITOK_CALL) {
        item = item->next;
    }
    return (ip_exec_stack_call_t *)item;
}

/**
 * @brief Find the execution stack item for a "REPEAT FOR" loop.
 *
 * @param[in] exec The execution context.
 * @param[in] node Points to the "REPEAT FOR" node in the abstract
 * syntax tree.
 *
 * @return A pointer to the loop's execution stack item, or NULL if
 * the loop does not exist in the current context.
 */
static ip_exec_stack_loop_t *ip_exec_find_loop
    (ip_exec_t *exec, ip_ast_node_t *node)
{
    /* Walk up the stack items until we find the loop or a call.
     * The call terminates the search because we cannot iterate
     * loops that started outside of the current subroutine. */
    ip_exec_stack_item_t *item = exec->stack;
    while (item && item->type != ITOK_CALL) {
        if (((ip_exec_stack_loop_t *)item)->node == node) {
            return (ip_exec_stack_loop_t *)item;
        }
        item = item->next;
    }
    return 0;
}

void ip_exec_free(ip_exec_t *exec)
{
    ip_exec_stack_item_t *stack;
    ip_exec_stack_item_t *next;
    ip_program_free(exec->program);
    ip_value_release(&(exec->this_value));
    stack = exec->stack;
    while (stack != 0) {
        next = stack->next;
        ip_exec_free_stack_item(stack);
        stack = next;
    }
    memset(exec, 0, sizeof(ip_exec_t));
}

void ip_exec_reset(ip_exec_t *exec)
{
    ip_value_set_unknown(&(exec->this_value));
    exec->pc = exec->program->statements.first;
    ip_program_reset_variables(exec->program);
}

/* Forward declaration */
static int ip_exec_eval_expression
    (ip_exec_t *exec, ip_ast_node_t *expr, ip_value_t *result);

/**
 * @brief Evaluates a unary operator on an integer argument.
 *
 * @param[out] result Return the result.
 * @param[in] x Value of the sub-expression.
 *
 * @return IP_EXEC_OK or an error code.
 */
typedef int (*ip_exec_unary_int_t)(ip_value_t *result, ip_int_t x);

/**
 * @brief Evaluates a unary operator on a floating-point argument.
 *
 * @param[out] result Return the result.
 * @param[in] x Value of the sub-expression.
 *
 * @return IP_EXEC_OK or an error code.
 */
typedef int (*ip_exec_unary_float_t)(ip_value_t *result, ip_float_t x);

/**
 * @brief Evaluates a unary operator on a string argument.
 *
 * @param[out] result Return the result.
 * @param[in] x Value of the sub-expression.
 *
 * @return IP_EXEC_OK or an error code.
 */
typedef int (*ip_exec_unary_string_t)(ip_value_t *result, ip_string_t *x);

/**
 * @brief Evaluates a binary operator on integer arguments.
 *
 * @param[out] result Return the result.
 * @param[in] x Value of the left sub-expression.
 * @param[in] y Value of the right sub-expression.
 *
 * @return IP_EXEC_OK or an error code.
 */
typedef int (*ip_exec_binary_int_t)
    (ip_int_t *result, ip_int_t x, ip_int_t y);

/**
 * @brief Evaluates a binary operator on floating-point arguments.
 *
 * @param[out] result Return the result.
 * @param[in] x Value of the left sub-expression.
 * @param[in] y Value of the right sub-expression.
 *
 * @return IP_EXEC_OK or an error code.
 */
typedef int (*ip_exec_binary_float_t)
    (ip_float_t *result, ip_float_t x, ip_float_t y);

/**
 * @brief Evaluates a binary operator on string arguments.
 *
 * @param[out] result Return the result.
 * @param[in] x Value of the left sub-expression.
 * @param[in] y Value of the right sub-expression.
 *
 * @return IP_EXEC_OK or an error code.
 */
typedef int (*ip_exec_binary_string_t)
    (ip_string_t **result, ip_string_t *x, ip_string_t *y);

#define IP_COND_ST  0x0001  /**< Condition result is smaller than */
#define IP_COND_EQ  0x0002  /**< Condition result is equal to */
#define IP_COND_GT  0x0004  /**< Condition result is greater than */

/**
 * @brief Evaluates a binary condition on integer arguments.
 *
 * @param[in] x Value of the left sub-expression.
 * @param[in] y Value of the right sub-expression.
 *
 * @return IP_COND_ST, IP_COND_EQ, or IP_COND_GT depending upon the
 * relationship between the arguments.
 */
typedef int (*ip_exec_binary_cond_int_t)(ip_int_t x, ip_int_t y);

/**
 * @brief Evaluates a binary condition on floating-point arguments.
 *
 * @param[in] x Value of the left sub-expression.
 * @param[in] y Value of the right sub-expression.
 *
 * @return IP_COND_ST, IP_COND_EQ, or IP_COND_GT depending upon the
 * relationship between the arguments.
 */
typedef int (*ip_exec_binary_cond_float_t)(ip_float_t x, ip_float_t y);

/**
 * @brief Evaluates a binary condition on string arguments.
 *
 * @param[in] x Value of the left sub-expression.
 * @param[in] y Value of the right sub-expression.
 *
 * @return IP_COND_ST, IP_COND_EQ, or IP_COND_GT depending upon the
 * relationship between the arguments.
 */
typedef int (*ip_exec_binary_cond_string_t)(ip_string_t *x, ip_string_t *y);

/**
 * @brief Evaluates a unary expression.
 *
 * @param[in,out] exec The execution context.
 * @param[in] expr The expression to be evaluated.
 * @param[out] result Returns the result.
 * @param[in,out] sub Temporary storage for the sub-expression's value.
 * @param[in] int_func Evaluates the expression with an integer argument.
 * @param[in] float_func Evaluates the expression with a floating-point argument.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_exec_eval_unary_expression
    (ip_exec_t *exec, ip_ast_node_t *expr, ip_value_t *result,
     ip_value_t *sub, ip_exec_unary_int_t int_func,
     ip_exec_unary_float_t float_func)
{
    int status;

    /* Evaluate the subexpression */
    status = ip_exec_eval_expression(exec, expr->children.left, sub);
    if (status != IP_EXEC_OK) {
        return status;
    }

    /* Evaluate the expression */
    if (sub->type == IP_TYPE_INT) {
        return (*int_func)(result, sub->ivalue);
    } else if (sub->type == IP_TYPE_STRING) {
        return IP_EXEC_BAD_TYPE;
    } else {
        status = ip_value_to_float(sub);
        if (status != IP_EXEC_OK) {
            return status;
        }
        return (*float_func)(result, sub->fvalue);
    }
    return IP_EXEC_OK;
}

/**
 * @brief Evaluates a unary expression on a string argument.
 *
 * @param[in,out] exec The execution context.
 * @param[in] expr The expression to be evaluated.
 * @param[out] result Returns the result.
 * @param[in,out] sub Temporary storage for the sub-expression's value.
 * @param[in] string_func Evaluates the expression with a string argument.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_exec_eval_unary_string_expression
    (ip_exec_t *exec, ip_ast_node_t *expr, ip_value_t *result,
     ip_value_t *sub, ip_exec_unary_string_t string_func)
{
    int status;

    /* Evaluate the subexpression */
    status = ip_exec_eval_expression(exec, expr->children.left, sub);
    if (status != IP_EXEC_OK) {
        return status;
    }

    /* Evaluate the expression */
    if (sub->type == IP_TYPE_STRING) {
        return (*string_func)(result, sub->svalue);
    } else {
        return IP_EXEC_BAD_TYPE;
    }
    return IP_EXEC_OK;
}

/**
 * @brief Evaluates a binary expression.
 *
 * @param[in,out] exec The execution context.
 * @param[in] expr The expression to be evaluated.
 * @param[out] result Returns the result.
 * @param[in,out] left Temporary storage for the left sub-expression's value.
 * @param[in,out] right Temporary storage for the right sub-expression's value.
 * @param[in] int_func Evaluates the expression with integer arguments.
 * @param[in] float_func Evaluates the expression with floating-point arguments.
 * @param[in] string_func Evaluates the expression with string arguments;
 * may be NULL if strings are not permitted.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_exec_eval_binary_expression
    (ip_exec_t *exec, ip_ast_node_t *expr, ip_value_t *result,
     ip_value_t *left, ip_value_t *right,
     ip_exec_binary_int_t int_func, ip_exec_binary_float_t float_func,
     ip_exec_binary_string_t string_func)
{
    int status;

    /* Evaluate the subexpressions */
    status = ip_exec_eval_expression(exec, expr->children.left, left);
    if (status != IP_EXEC_OK) {
        return status;
    }
    status = ip_exec_eval_expression(exec, expr->children.right, right);
    if (status != IP_EXEC_OK) {
        return status;
    }

    /* Cast the values to a common type (int, float, or string) */
    if (left->type == IP_TYPE_INT && right->type == IP_TYPE_INT) {
        ip_int_t ivalue;
        status = (*int_func)(&ivalue, left->ivalue, right->ivalue);
        if (status != IP_EXEC_OK) {
            return status;
        }
        ip_value_set_int(result, ivalue);
    } else if (left->type == IP_TYPE_STRING || right->type == IP_TYPE_STRING) {
        ip_string_t *svalue = 0;
        if (!string_func) {
            return IP_EXEC_BAD_TYPE;
        }
        status = ip_value_to_string(left);
        if (status != IP_EXEC_OK) {
            return status;
        }
        status = ip_value_to_string(right);
        if (status != IP_EXEC_OK) {
            return status;
        }
        status = (*string_func)(&svalue, left->svalue, right->svalue);
        if (status != IP_EXEC_OK) {
            return status;
        }
        ip_value_set_string(result, svalue);
        ip_string_deref(svalue);
    } else {
        ip_float_t fvalue;
        status = ip_value_to_float(left);
        if (status != IP_EXEC_OK) {
            return status;
        }
        status = ip_value_to_float(right);
        if (status != IP_EXEC_OK) {
            return status;
        }
        status = (*float_func)(&fvalue, left->fvalue, right->fvalue);
        if (status != IP_EXEC_OK) {
            return status;
        }
        ip_value_set_float(result, fvalue);
    }
    return IP_EXEC_OK;
}

/**
 * @brief Evaluates a binary condition.
 *
 * @param[in,out] exec The execution context.
 * @param[in] expr The expression to be evaluated.
 * @param[out] result Returns the result.
 * @param[in,out] left Temporary storage for the left sub-expression's value.
 * @param[in,out] right Temporary storage for the right sub-expression's value.
 * @param[in] int_func Evaluates the expression with integer arguments.
 * @param[in] float_func Evaluates the expression with floating-point arguments.
 * @param[in] string_func Evaluates the expression with string arguments;
 * may be NULL if strings are not permitted.
 * @param[in] expected Expected condition; e.g. IP_COND_ST | IP_COND_EQ
 * for smaller than or equal to.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_exec_eval_binary_condition
    (ip_exec_t *exec, ip_ast_node_t *expr, ip_value_t *result,
     ip_value_t *left, ip_value_t *right,
     ip_exec_binary_cond_int_t int_func,
     ip_exec_binary_cond_float_t float_func,
     ip_exec_binary_cond_string_t string_func, int expected)
{
    int status;
    int cmp;

    /* Evaluate the subexpressions */
    status = ip_exec_eval_expression(exec, expr->children.left, left);
    if (status != IP_EXEC_OK) {
        return status;
    }
    status = ip_exec_eval_expression(exec, expr->children.right, right);
    if (status != IP_EXEC_OK) {
        return status;
    }

    /* Cast the values to a common type (int, float, or string) */
    if (left->type == IP_TYPE_INT && right->type == IP_TYPE_INT) {
        cmp = (*int_func)(left->ivalue, right->ivalue);
    } else if (left->type == IP_TYPE_STRING ||
               right->type == IP_TYPE_STRING) {
        if (!string_func) {
            return IP_EXEC_BAD_TYPE;
        }
        status = ip_value_to_string(left);
        if (status != IP_EXEC_OK) {
            return status;
        }
        status = ip_value_to_string(right);
        if (status != IP_EXEC_OK) {
            return status;
        }
        cmp = (*string_func)(left->svalue, right->svalue);
    } else {
        status = ip_value_to_float(left);
        if (status != IP_EXEC_OK) {
            return status;
        }
        status = ip_value_to_float(right);
        if (status != IP_EXEC_OK) {
            return status;
        }
        cmp = (*float_func)(left->fvalue, right->fvalue);
    }
    ip_value_set_int(result, (cmp & expected) ? 1 : 0);
    return IP_EXEC_OK;
}

static int ip_eval_int_add(ip_int_t *result, ip_int_t x, ip_int_t y)
{
    *result = x + y;
    return IP_EXEC_OK;
}

static int ip_eval_float_add(ip_float_t *result, ip_float_t x, ip_float_t y)
{
    *result = x + y;
    return IP_EXEC_OK;
}

static int ip_eval_string_add
    (ip_string_t **result, ip_string_t *x, ip_string_t *y)
{
    *result = ip_string_concat(x, y);
    return IP_EXEC_OK;
}

static int ip_eval_int_sub(ip_int_t *result, ip_int_t x, ip_int_t y)
{
    *result = x - y;
    return IP_EXEC_OK;
}

static int ip_eval_float_sub(ip_float_t *result, ip_float_t x, ip_float_t y)
{
    *result = x - y;
    return IP_EXEC_OK;
}

static int ip_eval_int_mul(ip_int_t *result, ip_int_t x, ip_int_t y)
{
    *result = x * y;
    return IP_EXEC_OK;
}

static int ip_eval_float_mul(ip_float_t *result, ip_float_t x, ip_float_t y)
{
    *result = x * y;
    return IP_EXEC_OK;
}

static int ip_eval_int_div(ip_int_t *result, ip_int_t x, ip_int_t y)
{
    if (y != 0) {
        *result = x / y;
        return IP_EXEC_OK;
    } else {
        *result = 0;
        return IP_EXEC_DIV_ZERO;
    }
}

static int ip_eval_float_div(ip_float_t *result, ip_float_t x, ip_float_t y)
{
    *result = x / y;
    return IP_EXEC_OK;
}

static int ip_eval_int_mod(ip_int_t *result, ip_int_t x, ip_int_t y)
{
    if (y != 0) {
        *result = x % y;
        return IP_EXEC_OK;
    } else {
        *result = 0;
        return IP_EXEC_DIV_ZERO;
    }
}

static int ip_eval_float_mod(ip_float_t *result, ip_float_t x, ip_float_t y)
{
    *result = fmod(x, y);
    return IP_EXEC_OK;
}

static int ip_eval_int_is(ip_value_t *result, ip_int_t x)
{
    ip_value_set_int(result, x != 0);
    return IP_EXEC_OK;
}

static int ip_eval_float_is(ip_value_t *result, ip_float_t x)
{
    ip_value_set_int(result, x != 0);
    return IP_EXEC_OK;
}

static int ip_eval_int_is_not(ip_value_t *result, ip_int_t x)
{
    ip_value_set_int(result, x == 0);
    return IP_EXEC_OK;
}

static int ip_eval_float_is_not(ip_value_t *result, ip_float_t x)
{
    ip_value_set_int(result, x == 0);
    return IP_EXEC_OK;
}

static int ip_eval_int_cmp(ip_int_t x, ip_int_t y)
{
    if (x < y) {
        return IP_COND_ST;
    } else if (x > y) {
        return IP_COND_GT;
    } else {
        return IP_COND_EQ;
    }
}

static int ip_eval_float_cmp(ip_float_t x, ip_float_t y)
{
    if (x < y) {
        return IP_COND_ST;
    } else if (x > y) {
        return IP_COND_GT;
    } else {
        return IP_COND_EQ;
    }
}

static int ip_eval_string_cmp(ip_string_t *x, ip_string_t *y)
{
    int cmp;
    if (!x) {
        return y ? IP_COND_ST : IP_COND_EQ;
    } else if (!y) {
        return IP_COND_GT;
    }
    cmp = strcmp(x->data, y->data);
    if (cmp < 0) {
        return IP_COND_ST;
    } else if (cmp > 0) {
        return IP_COND_GT;
    } else {
        return IP_COND_EQ;
    }
}

static int ip_eval_int_much_gt(ip_int_t x, ip_int_t y)
{
    return ip_eval_int_cmp(x, y);
}

static int ip_eval_float_much_gt(ip_float_t x, ip_float_t y)
{
    /* Interpret "MUCH GREATER THAN" as "1 million times larger" like CSIRAC */
    if (x > y && (x * 0.000001) >= y) {
        return IP_COND_GT;
    } else {
        return IP_COND_ST;
    }
}

static int ip_eval_int_much_st(ip_int_t x, ip_int_t y)
{
    return ip_eval_int_cmp(x, y);
}

static int ip_eval_float_much_st(ip_float_t x, ip_float_t y)
{
    /* Interpret "MUCH SMALLER THAN" as "1 million times smaller" like CSIRAC */
    if (x < y && x <= (y * 0.000001)) {
        return IP_COND_ST;
    } else {
        return IP_COND_GT;
    }
}

static int ip_eval_int_zero(ip_value_t *result, ip_int_t x)
{
    ip_value_set_int(result, x == 0);
    return IP_EXEC_OK;
}

static int ip_eval_float_zero(ip_value_t *result, ip_float_t x)
{
    /* Classic INTERPROGRAM interpreted "IF X IS ZERO" as meaning
     * "X is very close to zero" rather than "X is exactly zero". */
    ip_value_set_int(result, fabs(x) < IP_FLOAT_EPSILON);
    return IP_EXEC_OK;
}

static int ip_eval_int_positive(ip_value_t *result, ip_int_t x)
{
    ip_value_set_int(result, x >= 0);
    return IP_EXEC_OK;
}

static int ip_eval_float_positive(ip_value_t *result, ip_float_t x)
{
    ip_value_set_int(result, x >= 0);
    return IP_EXEC_OK;
}

static int ip_eval_int_negative(ip_value_t *result, ip_int_t x)
{
    ip_value_set_int(result, x < 0);
    return IP_EXEC_OK;
}

static int ip_eval_float_negative(ip_value_t *result, ip_float_t x)
{
    ip_value_set_int(result, x < 0);
    return IP_EXEC_OK;
}

static int ip_eval_int_finite(ip_value_t *result, ip_int_t x)
{
    (void)x;
    ip_value_set_int(result, 1);
    return IP_EXEC_OK;
}

static int ip_eval_float_finite(ip_value_t *result, ip_float_t x)
{
    ip_value_set_int(result, finite(x));
    return IP_EXEC_OK;
}

static int ip_eval_int_infinite(ip_value_t *result, ip_int_t x)
{
    (void)x;
    ip_value_set_int(result, 0);
    return IP_EXEC_OK;
}

static int ip_eval_float_infinite(ip_value_t *result, ip_float_t x)
{
    ip_value_set_int(result, isinf(x));
    return IP_EXEC_OK;
}

static int ip_eval_int_a_number(ip_value_t *result, ip_int_t x)
{
    (void)x;
    ip_value_set_int(result, 1);
    return IP_EXEC_OK;
}

static int ip_eval_float_a_number(ip_value_t *result, ip_float_t x)
{
    ip_value_set_int(result, !isnan(x));
    return IP_EXEC_OK;
}

static int ip_eval_string_empty(ip_value_t *result, ip_string_t *x)
{
    ip_value_set_int(result, !x || x->len == 0);
    return IP_EXEC_OK;
}

static int ip_eval_string_length(ip_value_t *result, ip_string_t *x)
{
    ip_value_set_int(result, x ? x->len : 0);
    return IP_EXEC_OK;
}

/**
 * @brief Evaluates an expression.
 *
 * @param[in,out] exec The execution context.
 * @param[in] expr The expression to be evaluated.
 * @param[out] result Returns the result.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_exec_eval_expression
    (ip_exec_t *exec, ip_ast_node_t *expr, ip_value_t *result)
{
    int status = IP_EXEC_OK;
    ip_value_t left;
    ip_value_t right;

    /* If the node is NULL, we cannot evaluate this expression */
    if (!expr) {
        ip_value_set_unknown(result);
        return IP_EXEC_BAD_TYPE;
    }

    /* Initialise the sub-expression results */
    ip_value_init(&left);
    ip_value_init(&right);

    /* Determine what to do based on the type of expression node */
    switch (expr->type) {
    case ITOK_THIS:
        /* Get the current value of the "THIS" variable */
        ip_value_assign(result, &(exec->this_value));
        break;

    case ITOK_VAR_NAME:
        /* Get the value of a variable */
        status = ip_value_from_var(result, expr->var);
        break;

    case ITOK_INT_VALUE:
        /* Integer constant */
        ip_value_set_int(result, expr->ivalue);
        break;

    case ITOK_FLOAT_VALUE:
        /* Floating-point constant */
        ip_value_set_float(result, expr->fvalue);
        break;

    case ITOK_STR_VALUE:
        /* String constant */
        ip_value_set_string(result, expr->text);
        break;

    case ITOK_TO_INT:
        /* Convert the sub-expression's value into an integer */
        status = ip_exec_eval_expression(exec, expr->children.left, result);
        if (status == IP_EXEC_OK) {
            status = ip_value_to_int(result);
        }
        break;

    case ITOK_TO_FLOAT:
        /* Convert the sub-expression's value into a floating-point value */
        status = ip_exec_eval_expression(exec, expr->children.left, result);
        if (status == IP_EXEC_OK) {
            status = ip_value_to_float(result);
        }
        break;

    case ITOK_TO_STRING:
        /* Convert the sub-expression's value into a string value */
        status = ip_exec_eval_expression(exec, expr->children.left, result);
        if (status == IP_EXEC_OK) {
            status = ip_value_to_string(result);
        }
        break;

    case ITOK_TO_DYNAMIC:
        /* Convert the sub-expression's value into a dynamic value.
         * Which is equivalent to just evaluating the sub-expression
         * as all ip_value_t values are implicitly dynamic. */
        status = ip_exec_eval_expression(exec, expr->children.left, result);
        break;

    case ITOK_INDEX_INT:
    case ITOK_INDEX_FLOAT:
    case ITOK_INDEX_STRING:
        /* Index into an array */
        status = ip_exec_eval_expression(exec, expr->children.right, &right);
        if (status == IP_EXEC_OK) {
            status = ip_value_to_int(&right);
            if (status == IP_EXEC_OK) {
                status = ip_value_from_array
                    (result, expr->children.left->var, right.ivalue);
            }
        }
        break;

#define EVAL_UNARY(name) \
    status = ip_exec_eval_unary_expression \
        ((exec), (expr), (result), &left, ip_eval_int_##name, \
         ip_eval_float_##name)
#define EVAL_UNARY_STRING(name) \
    status = ip_exec_eval_unary_string_expression \
        ((exec), (expr), (result), &left, ip_eval_string_##name)
#define EVAL_BINARY(name) \
    status = ip_exec_eval_binary_expression \
        ((exec), (expr), (result), &left, &right, ip_eval_int_##name, \
         ip_eval_float_##name, 0)
#define EVAL_BINARY_STRING(name) \
    status = ip_exec_eval_binary_expression \
        ((exec), (expr), (result), &left, &right, ip_eval_int_##name, \
         ip_eval_float_##name, ip_eval_string_##name)
#define EVAL_BINARY_CONDITION(op, cond) \
    status = ip_exec_eval_binary_condition \
        ((exec), (expr), (result), &left, &right, ip_eval_int_##op, \
         ip_eval_float_##op, 0, (cond))
#define EVAL_BINARY_CONDITION_STRING(op, cond) \
    status = ip_exec_eval_binary_condition \
        ((exec), (expr), (result), &left, &right, ip_eval_int_##op, \
         ip_eval_float_##op, ip_eval_string_##op, (cond))

    case ITOK_ADD:
    case ITOK_PLUS:
        EVAL_BINARY_STRING(add);
        break;

    case ITOK_SUBTRACT:
    case ITOK_MINUS:
        EVAL_BINARY(sub);
        break;

    case ITOK_MULTIPLY:
    case ITOK_MUL:
        EVAL_BINARY(mul);
        break;

    case ITOK_DIVIDE:
    case ITOK_DIV:
        EVAL_BINARY(div);
        break;

    case ITOK_MODULO:
        EVAL_BINARY(mod);
        break;

    case ITOK_IS:
        EVAL_UNARY(is);
        break;

    case ITOK_IS_NOT:
        EVAL_UNARY(is_not);
        break;

    case ITOK_GREATER_THAN:
        EVAL_BINARY_CONDITION_STRING(cmp, IP_COND_GT);
        break;

    case ITOK_MUCH_GREATER_THAN:
        EVAL_BINARY_CONDITION(much_gt, IP_COND_GT);
        break;

    case ITOK_SMALLER_THAN:
        EVAL_BINARY_CONDITION_STRING(cmp, IP_COND_ST);
        break;

    case ITOK_MUCH_SMALLER_THAN:
        EVAL_BINARY_CONDITION(much_st, IP_COND_ST);
        break;

    case ITOK_ZERO:
        EVAL_UNARY(zero);
        break;

    case ITOK_POSITIVE:
        EVAL_UNARY(positive);
        break;

    case ITOK_NEGATIVE:
        EVAL_UNARY(negative);
        break;

    case ITOK_EQUAL_TO:
        EVAL_BINARY_CONDITION_STRING(cmp, IP_COND_EQ);
        break;

    case ITOK_GREATER_OR_EQUAL:
        EVAL_BINARY_CONDITION_STRING(cmp, IP_COND_GT | IP_COND_EQ);
        break;

    case ITOK_SMALLER_OR_EQUAL:
        EVAL_BINARY_CONDITION_STRING(cmp, IP_COND_ST | IP_COND_EQ);
        break;

    case ITOK_FINITE:
        EVAL_UNARY(finite);
        break;

    case ITOK_INFINITE:
        EVAL_UNARY(infinite);
        break;

    case ITOK_A_NUMBER:
        EVAL_UNARY(a_number);
        break;

    case ITOK_EMPTY:
        EVAL_UNARY_STRING(empty);
        break;

    case ITOK_LENGTH_OF:
        EVAL_UNARY_STRING(length);
        break;

    case ITOK_FUNCTION_INVOKE:
        /* Invoke a built-in library function */
        if (expr->children.right) {
            status = ip_exec_eval_expression
                (exec, expr->children.right, result);
            if (status == IP_EXEC_OK) {
                ip_builtin_handler_t handler =
                    (ip_builtin_handler_t)(expr->children.left->builtin_handler);
                status = (*handler)(exec, result, 1);
            }
        } else {
            ip_builtin_handler_t handler =
                (ip_builtin_handler_t)(expr->children.left->builtin_handler);
            ip_value_release(result); /* Set to unknown */
            status = (*handler)(exec, result, 1);
        }
        break;

    case ITOK_ARG_NUMBER: {
        /* Reference to a local variable in the current subroutine */
        ip_exec_stack_call_t *frame = ip_exec_find_call(exec);
        if (frame) {
            ip_value_assign(result, &(frame->locals[expr->ivalue]));
            if (result->type == IP_TYPE_UNKNOWN) {
                status = IP_EXEC_UNINIT;
            }
        } else {
            /* Not currently within a subroutine */
            status = IP_EXEC_BAD_LOCAL;
        }
        break; }

    default:
        /* Don't know what kind of expression this is */
        ip_value_set_unknown(result);
        status = IP_EXEC_BAD_TYPE;
        break;
    }

    /* Clean up and exit */
    ip_value_release(&left);
    ip_value_release(&right);
    return status;
}

/**
 * @brief Evaluates a boolean condition.
 *
 * @param[in,out] exec The execution context.
 * @param[in] expr The expression to evaluate.
 *
 * @return IP_EXEC_OK if the condition is true, IP_EXEC_FALSE if the
 * condition is false, or an error code otherwise.
 */
static int ip_exec_eval_condition(ip_exec_t *exec, ip_ast_node_t *expr)
{
    ip_value_t condition;
    int status;

    /* Evaluate the condition */
    ip_value_init(&condition);
    status = ip_exec_eval_expression(exec, expr, &condition);
    if (status != IP_EXEC_OK) {
        ip_value_release(&condition);
        return status;
    }

    /* Determine if the condition is true or false or invalid */
    switch (condition.type) {
    case IP_TYPE_INT:
        if (condition.ivalue == 0) {
            status = IP_EXEC_FALSE;
        }
        break;

    case IP_TYPE_FLOAT:
        if (condition.fvalue == 0) {
            status = IP_EXEC_FALSE;
        }
        break;

    default:
        status = IP_EXEC_BAD_TYPE;
        break;
    }

    /* Clean up and exit */
    ip_value_release(&condition);
    return status;
}

/**
 * @brief Assigns a value to a variable.
 *
 * @param[in,out] exec The execution context.
 * @param[in] node The node that corresponds to the variable.
 * @param[in] value The value to assign to the variable.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_exec_assign_variable
    (ip_exec_t *exec, ip_ast_node_t *node, ip_value_t *value)
{
    ip_value_t index;
    int status;

    /* Is this a variable or array assignment? */
    if (node->type == ITOK_VAR_NAME) {
        /* Assign to an ordinary variable */
        status = ip_value_to_var(node->var, value);
    } else if (node->type == ITOK_INDEX_INT ||
               node->type == ITOK_INDEX_FLOAT ||
               node->type == ITOK_INDEX_STRING) {
        /* Assign to an array element; first evaluate the array index */
        ip_value_init(&index);
        status = ip_exec_eval_expression(exec, node->children.right, &index);
        if (status == IP_EXEC_OK) {
            status = ip_value_to_int(&index);
            if (status == IP_EXEC_OK) {
                /* Finally assign to the array element */
                status = ip_value_to_array
                    (node->children.left->var, index.ivalue, value);
            }
        }
        ip_value_release(&index);
    } else if (node->type == ITOK_ARG_NUMBER) {
        /* Assign to a local variable in the current stack frame */
        ip_exec_stack_call_t *frame = ip_exec_find_call(exec);
        if (frame) {
            ip_value_assign(&(frame->locals[node->ivalue]), value);
            status = IP_EXEC_OK;
        } else {
            /* Cannot assign local variables at the global level */
            status = IP_EXEC_BAD_LOCAL;
        }
    } else {
        /* Cannot assign to this type of variable */
        status = IP_EXEC_BAD_TYPE;
    }
    return status;
}

/**
 * @brief Performs an assignment statement.
 *
 * @param[in,out] exec The execution context.
 * @param[in] node The assignment node.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_exec_assignment_statement(ip_exec_t *exec, ip_ast_node_t *node)
{
    ip_value_t value;
    int status;

    /* Get the value to be assigned to the variable */
    ip_value_init(&value);
    if (node->children.right) {
        /* Evaluate the expression value to be assigned for "SET" */
        status = ip_exec_eval_expression(exec, node->children.right, &value);
        if (status != IP_EXEC_OK) {
            ip_value_release(&value);
            return status;
        }
    } else {
        /* No expression, so this is a "REPLACE" with value "THIS" */
        ip_value_assign(&value, &(exec->this_value));
    }

    /* Assign the value to the variable */
    status = ip_exec_assign_variable(exec, node->children.left, &value);
    ip_value_release(&value);
    return status;
}

/**
 * @brief Writes tilde characters that are not part of a separator.
 *
 * @param[in] exec The execution context.
 * @param[in] count The number of tilde characters to write.
 */
static void ip_exec_write_tildes(ip_exec_t *exec, size_t count)
{
    while (count > 0) {
        if (exec->output_char) {
            (*(exec->output_char))(exec, '~');
        } else {
            fputc('~', exec->output);
        }
        --count;
    }
}

/**
 * @brief Copies the contents of an input stream to an output stream,
 * until "~~~~~" or EOF.
 *
 * @param[in,out] exec The execution context.
 * @param[in] input The input stream.
 * @param[in] ignore_output Non-zero to drop the data rather than output it.
 */
static void ip_exec_copy_tape(ip_exec_t *exec, FILE *input, int ignore_output)
{
    size_t tilde_count = 0;
    int ch;
    if (exec->program->next_input) {
        /* Read from the embedded input data in the program */
        if (exec->program->next_input[0] == '\0') {
            /* We have reached the end of the embedded input.
             * Report EOF and switch to using exec->input next time. */
            exec->program->next_input = 0;
            return;
        }

        /* Copy characters from the embedded input until "~~~~~" or EOF */
        while ((ch = *(exec->program->next_input)++) != '\0') {
            if (ch == '~') {
                ++tilde_count;
                if (tilde_count >= 5) {
                    /* We have found the "~~~~~" separator.  It may be
                     * followed by more tilde's, so deal with them too. */
                    while ((ch = *(exec->program->next_input)++) != '\0') {
                        if (ch != '~') {
                            --(exec->program->next_input);
                            break;
                        }
                    }
                    if (exec->program->next_input[0] == '\n') {
                        /* Skip the EOL after the tildes if present */
                        ++(exec->program->next_input);
                    }
                    return;
                }
            } else {
                /* Non-tilde character; output it directly */
                if (!ignore_output) {
                    if (tilde_count > 0) {
                        ip_exec_write_tildes(exec, tilde_count);
                    }
                    if (exec->output_char) {
                        (*(exec->output_char))(exec, ch);
                    } else {
                        fputc(ch, exec->output);
                    }
                }
                tilde_count = 0;
            }
        }
        if (tilde_count > 0 && !ignore_output) {
            ip_exec_write_tildes(exec, tilde_count);
        }
        return;
    }
    while ((ch = fgetc(input)) != EOF) {
        if (ch == '~') {
            ++tilde_count;
            if (tilde_count >= 5) {
                /* We have found the "~~~~~" separator.  It may be
                 * followed by more tilde's, so deal with them too. */
                while ((ch = fgetc(input)) != EOF) {
                    if (ch != '~') {
                        /* Skip the EOL after the tildes if present */
                        if (ch == '\r') {
                            ch = fgetc(input);
                            if (ch != '\n' && ch != EOF) {
                                ungetc(ch, input);
                            }
                        } else if (ch != '\n') {
                            ungetc(ch, input);
                        }
                        break;
                    }
                }
                return;
            }
        } else {
            /* Non-tilde character; output it directly */
            if (!ignore_output) {
                if (tilde_count > 0) {
                    ip_exec_write_tildes(exec, tilde_count);
                }
                if (exec->output_char) {
                    (*(exec->output_char))(exec, ch);
                } else {
                    fputc(ch, exec->output);
                }
            }
            tilde_count = 0;
        }
    }
    if (tilde_count > 0 && !ignore_output) {
        ip_exec_write_tildes(exec, tilde_count);
    }
}

/**
 * @brief Jumps to a specific label in the program.
 *
 * @param[in,out] exec The execution context.
 * @param[in] node The node representing the label.
 * @param[in] call Non-zero if this is a "CALL"; zero for "GO TO".
 * @param[in] num_args Number of arguments that were passed to the call.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_exec_jump_to_label
    (ip_exec_t *exec, ip_ast_node_t *node, int call, int num_args)
{
    int status = IP_EXEC_OK;
    if (node->type == ITOK_LABEL) {
        if (node->label->node) {
            /* Fetch the destination node direct from the label */
            exec->pc = node->label->node;
        } else if (node->label->builtin && call) {
            /* Calling a built-in statement */
            ip_exec_stack_call_t *frame = ip_exec_find_call(exec);
            ip_builtin_handler_t handler;
            handler = (ip_builtin_handler_t)(node->label->builtin);
            status = (*handler)(exec, frame->locals, num_args);

            /* Pop the top-most call frame which we don't need any more */
            ip_exec_pop_stack_to(exec, frame->base.next);
        } else {
            /* Cannot use "GO TO" with a built-in statement */
            status = IP_EXEC_BAD_LABEL;
        }
    } else {
        /* Need to evaluate the expression (computed goto) */
        ip_value_t value;
        ip_value_init(&value);
        status = ip_exec_eval_expression(exec, node, &value);
        if (status == IP_EXEC_OK) {
            if (value.type == IP_TYPE_INT) {
                /* Look up the label by number */
                ip_label_t *label = ip_label_lookup_by_number
                    (&(exec->program->labels), value.ivalue);
                if (label) {
                    /* Jump to the node associated with the label */
                    exec->pc = label->node;
                } else {
                    /* Unknown label */
                    status = IP_EXEC_BAD_LABEL;
                }
            } else {
                /* Labels must be integers when doing a computed goto */
                status = IP_EXEC_BAD_TYPE;
            }
        }
        ip_value_release(&value);
    }
    return status;
}

/**
 * @brief "REPEAT FROM" loop construct.
 *
 * @param[in,out] exec The execution context.
 * @param[in] node The "REPEAT FROM" node in the program.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_exec_repeat_from(ip_exec_t *exec, ip_ast_node_t *node)
{
    ip_var_t *var = node->children.right->var;
    if (ip_var_get_type(var) != IP_TYPE_INT) {
        /* Loop variable must be an integer */
        return IP_EXEC_BAD_TYPE;
    }
    if (var->ivalue == 0) {
        /* Loop variable is zero, so the loop now ends */
        return IP_EXEC_OK;
    } else if (var->ivalue > 0) {
        /* Decrement the loop variable towards zero */
        --(var->ivalue);
    } else {
        /* Increment the loop variable towards zero */
        ++(var->ivalue);
    }
    return ip_exec_jump_to_label(exec, node->children.left, 0, 0);
}

/**
 * @brief "REPEAT WHILE" loop construct.
 *
 * @param[in,out] exec The execution context.
 * @param[in] node The "REPEAT WHILE" node in the program.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_exec_repeat_while(ip_exec_t *exec, ip_ast_node_t *node)
{
    int status = ip_exec_eval_condition(exec, node->children.left);
    if (status == IP_EXEC_FALSE) {
        /* Condition is false, so jump to just past the
         * matching 'END REPEAT' at the end of the loop */
        exec->pc = node->children.right;
        if (exec->pc) {
            exec->pc = exec->pc->next;
        }
        status = IP_EXEC_OK;
    }
    return status;
}

/**
 * @brief Determine if we have reached the end of the iteration sequence.
 *
 * @param[in] exec The execution context.
 * @param[in] loop The loop.
 *
 * @return Non-zero if the loop is done, or zero for more iterations.
 */
static int ip_exec_is_loop_done(ip_exec_t *exec, ip_exec_stack_loop_t *loop)
{
    ip_value_t value;
    int status;

    /* Get the current value of the loop variable */
    ip_value_init(&value);
    status = ip_exec_eval_expression(exec, loop->var, &value);
    if (status != IP_EXEC_OK || value.type != loop->end.type) {
        /* Shouldn't happen, but end the loop anyway */
        ip_value_release(&value);
        return 1;
    }

    /* If the step is negative, then check if the variable's value is less
     * than the end.  Otherwise check if it is greater than the end */
    if (value.type == IP_TYPE_INT) {
        if (loop->step.ivalue < 0) {
            status = (value.ivalue < loop->end.ivalue);
        } else {
            status = (value.ivalue > loop->end.ivalue);
        }
    } else if (value.type == IP_TYPE_FLOAT) {
        if (loop->step.fvalue < 0) {
            status = (value.fvalue < loop->end.fvalue);
        } else {
            status = (value.fvalue > loop->end.fvalue);
        }
    } else {
        /* Cannot iterate over strings!  End the loop immeditely */
        status = 1;
    }
    ip_value_release(&value);
    return status;
}

/**
 * @brief Start of a "REPEAT FOR" loop construct.
 *
 * @param[in,out] exec The execution context.
 * @param[in] node The "REPEAT FOR" node in the program.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_exec_repeat_for(ip_exec_t *exec, ip_ast_node_t *node)
{
    ip_exec_stack_loop_t *loop;
    ip_ast_node_t *assign;
    ip_ast_node_t *var;
    ip_ast_node_t *end;
    ip_ast_node_t *step;
    int status;

    /* Break the "REPEAT FOR" node into individual pieces */
    step = node->children.left->children.right;
    end = node->children.left->children.left->children.right;
    assign = node->children.left->children.left->children.left;
    var = assign->children.left;

    /* Construct an execution stack item for the loop */
    loop = calloc(1, sizeof(ip_exec_stack_loop_t));
    if (!loop) {
        ip_out_of_memory();
    }
    loop->base.type = ITOK_REPEAT_FOR;
    loop->node = node;
    loop->var = var;

    /* Evaluate the end expression and step expression.  We evaluate these
     * before the variable assignment in case they involve the variable. */
    status = ip_exec_eval_expression(exec, end, &(loop->end));
    if (status == IP_EXEC_OK) {
        status = ip_exec_eval_expression(exec, step, &(loop->step));
    }
    if (status != IP_EXEC_OK) {
        ip_exec_free_stack_item(&(loop->base));
        return status;
    }
    if (loop->end.type != loop->step.type) {
        /* End and step values should be the same type */
        ip_exec_free_stack_item(&(loop->base));
        return IP_EXEC_BAD_TYPE;
    }

    /* Perform the assignment to the loop variable */
    status = ip_exec_assignment_statement(exec, assign);
    if (status != IP_EXEC_OK) {
        ip_exec_free_stack_item(&(loop->base));
        return status;
    }

    /* Check if the variable's original value is already past the end.
     * If it is, then jump to the end of the loop and stop. */
    if (ip_exec_is_loop_done(exec, loop)) {
        ip_exec_free_stack_item(&(loop->base));
        exec->pc = node->children.right->next;
        return IP_EXEC_OK;
    }

    /* Push the loop onto the execution stack and return */
    loop->base.next = exec->stack;
    exec->stack = &(loop->base);
    return IP_EXEC_OK;
}

/**
 * @brief "END REPEAT" at the end of a "REPEAT WHILE" construct.
 *
 * @param[in,out] exec The execution context.
 * @param[in] loop Reference to the "REPEAT FOR" execution stack item.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_exec_repeat_for_next(ip_exec_t *exec, ip_exec_stack_loop_t *loop)
{
    int status;
    ip_value_t value;
    ip_value_t step;

    /* Initialise the values we will be using */
    ip_value_init(&value);
    ip_value_init(&step);

    /* Add the step value to the loop variable */
    status = ip_exec_eval_expression(exec, loop->var, &value);
    if (status == IP_EXEC_OK) {
        ip_value_assign(&step, &(loop->step));
        if (value.type == IP_TYPE_INT) {
            status = ip_value_to_int(&step);
        } else if (value.type == IP_TYPE_FLOAT) {
            status = ip_value_to_float(&step);
        } else {
            status = IP_EXEC_BAD_TYPE;
        }
        if (status == IP_EXEC_OK) {
            if (value.type == IP_TYPE_INT) {
                value.ivalue += step.ivalue;
            } else {
                value.fvalue += step.fvalue;
            }
            status = ip_exec_assign_variable(exec, loop->var, &value);
            if (status == IP_EXEC_OK) {
                /* Check if we have passed the end point for the loop */
                if (!ip_exec_is_loop_done(exec, loop)) {
                    /* Continue execution from the node after the
                     * "REPEAT FOR" node for the loop. */
                    ip_value_release(&value);
                    ip_value_release(&step);
                    exec->pc = loop->node->next;
                    return IP_EXEC_OK;
                }
            }
        }
    }

    /* Loop is finished, so pop the execution stack item and continue
     * execution from the next statement after the "REPEAT FOR". */
    ip_exec_pop_stack_to(exec, loop->base.next);

    /* Clean up and exit */
    ip_value_release(&value);
    ip_value_release(&step);
    return status;
}

/**
 * @brief Reads a string from the input stream.
 *
 * @param[in] exec The execution context.
 *
 * @return The string that was read or NULL on EOF.
 */
static ip_string_t *ip_exec_read_string(ip_exec_t *exec)
{
    char buffer[BUFSIZ];
    size_t len;
    if (exec->input_line) {
        /* Input has been redirected to the console */
        (*(exec->input_line))(exec, buffer, sizeof(buffer));
    } else if (!fgets(buffer, sizeof(buffer), exec->input)) {
        return 0;
    }
    len = strlen(buffer);
    while (len > 0 &&
           (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
        /* Strip trailing end of line characters */
        --len;
    }
    return ip_string_create_with_length(buffer, len);
}

/**
 * @brief Reads an integer from the input stream.
 *
 * @param[in] exec The execution context.
 * @param[out] value Returns the integer value.
 *
 * @return The result from scanf().
 */
static int ip_exec_read_integer(ip_exec_t *exec, ip_int_t *value)
{
    char buffer[100];
    int scan_result;
    *value = 0;
    if (exec->input_line) {
        /* Input has been redirected to the console */
        (*(exec->input_line))(exec, buffer, sizeof(buffer));
        if (sizeof(ip_int_t) == 2) {
            int16_t i16value = 0;
            scan_result = sscanf(buffer, "%" SCNd16, &i16value);
            *value = i16value;
        } else if (sizeof(ip_int_t) == 4) {
            int32_t i32value = 0;
            scan_result = sscanf(buffer, "%" SCNd32, &i32value);
            *value = i32value;
        } else {
            scan_result = sscanf(buffer, "%" SCNd64, value);
        }
    } else {
        if (sizeof(ip_int_t) == 2) {
            int16_t i16value = 0;
            scan_result = fscanf(exec->input, "%" SCNd16, &i16value);
            *value = i16value;
        } else if (sizeof(ip_int_t) == 4) {
            int32_t i32value = 0;
            scan_result = fscanf(exec->input, "%" SCNd32, &i32value);
            *value = i32value;
        } else {
            scan_result = fscanf(exec->input, "%" SCNd64, value);
        }
    }
    return scan_result;
}

/**
 * @brief Reads a floating-point value from the input stream.
 *
 * @param[in] exec The execution context.
 * @param[out] value Returns the floating-point value.
 *
 * @return The result from scanf().
 */
static int ip_exec_read_float(ip_exec_t *exec, ip_float_t *value)
{
    char buffer[100];
    int scan_result;
    *value = 0;
    if (exec->input_line) {
        /* Input has been redirected to the console */
        (*(exec->input_line))(exec, buffer, sizeof(buffer));
        scan_result = sscanf(buffer, "%lf", value) ;
    } else {
        scan_result = fscanf(exec->input, "%lf", value) ;
    }
    return scan_result;
}

static void ip_exec_input_skip_spaces(ip_exec_t *exec)
{
    const char *input = exec->program->next_input;
    int ch;
    while ((ch = *input) != '\0') {
        if (ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\n') {
            ++input;
        } else {
            break;
        }
    }
    exec->program->next_input = input;
}

/**
 * @brief Reads a value from the input into a variable and "THIS".
 *
 * @param[in,out] exec The execution context.
 * @param[in] node Reference to the "INPUT" node.
 *
 * @return IP_EXEC_OK or an error code.
 *
 * If end of file is reached, the next statement in the code is skipped.
 */
static int ip_exec_input(ip_exec_t *exec, ip_ast_node_t *node)
{
    ip_value_t value;
    int status = IP_EXEC_OK;
    int eof = 0;
    int scan_result;
    int skip_eol = 1;
    int ch;
    int64_t ivalue;
    double fvalue;
    char *end;

    /* Read the value from the input source */
    ip_value_init(&value);
    switch (node->children.left->value_type) {
    case IP_TYPE_INT:
        /* Read an integer value */
        if (exec->program->next_input) {
            /* Read from the embedded input data in the program */
            ip_exec_input_skip_spaces(exec);
            if (exec->program->next_input[0] == '\0') {
                /* We have reached the end of the embedded input.
                 * Report EOF and switch to using exec->input next time. */
                exec->program->next_input = 0;
                eof = 1;
                skip_eol = 0;
            } else {
                long long llvalue =
                    strtoll(exec->program->next_input, &end, 10);
                if (!llvalue && end == exec->program->next_input) {
                    /* Invalid number in the embedded input */
                    status = IP_EXEC_BAD_INPUT;
                    break;
                }
                exec->program->next_input = end;
                ip_value_set_int(&value, (ip_int_t)llvalue);
            }
            break;
        }
        scan_result = ip_exec_read_integer(exec, &ivalue);
        if (scan_result < 0) {
            eof = 1;
        } else if (scan_result == 0) {
            status = IP_EXEC_BAD_INPUT;
        } else {
            ip_value_set_int(&value, ivalue);
        }
        break;

    case IP_TYPE_FLOAT:
        /* Read a floating-point value.  Note: Classic INTERPROGRAM
         * uses notation like "1.23(45)" to indicate a number with an
         * exponent.  However, fscanf() doesn't do this so any existing
         * input data needs to be modified to use "e" notation instead. */
        if (exec->program->next_input) {
            /* Read from the embedded input data in the program */
            ip_exec_input_skip_spaces(exec);
            if (exec->program->next_input[0] == '\0') {
                /* We have reached the end of the embedded input.
                 * Report EOF and switch to using exec->input next time. */
                exec->program->next_input = 0;
                eof = 1;
                skip_eol = 0;
            } else {
                fvalue = strtod(exec->program->next_input, &end);
                if (fvalue == 0.0 && end == exec->program->next_input) {
                    /* Invalid number in the embedded input */
                    status = IP_EXEC_BAD_INPUT;
                    break;
                }
                exec->program->next_input = end;
                ip_value_set_float(&value, (ip_float_t)fvalue);
            }
            break;
        }
        scan_result = ip_exec_read_float(exec, &fvalue);
        if (scan_result < 0) {
            eof = 1;
        } else if (scan_result == 0) {
            status = IP_EXEC_BAD_INPUT;
        } else {
            ip_value_set_float(&value, fvalue);
        }
        break;

    case IP_TYPE_STRING:
        /* Read a string value; every character until the next newline */
        value.type = IP_TYPE_STRING;
        skip_eol = 0;
        if (exec->program->next_input) {
            /* Read from the embedded input data in the program */
            if (exec->program->next_input[0] == '\0') {
                /* We have reached the end of the embedded input.
                 * Report EOF and switch to using exec->input next time. */
                exec->program->next_input = 0;
                value.svalue = ip_string_create_empty();
                eof = 1;
            } else {
                /* Read characters from the embedded input until EOL */
                end = strchr(exec->program->next_input, '\n');
                if (end) {
                    value.svalue = ip_string_create_with_length
                        (exec->program->next_input,
                         end - exec->program->next_input);
                    ++end;
                } else {
                    end = strchr(exec->program->next_input, '\0');
                    value.svalue = ip_string_create_with_length
                        (exec->program->next_input,
                         end - exec->program->next_input);
                }
                exec->program->next_input = end;
            }
            break;
        }
        value.svalue = ip_exec_read_string(exec);
        if (!(value.svalue)) {
            eof = 1;
            value.svalue = ip_string_create_empty();
        }
        break;

    default:
        ip_value_release(&value);
        return IP_EXEC_BAD_TYPE;
    }

    /* After a number, skip the following EOL; after a string, don't do this.
     * This ensures that if the previous line ended in a number, reading a
     * string will read a new line instead of the '\n' on the previous line. */
    if (skip_eol) {
        if (exec->program->next_input) {
            if (exec->program->next_input[0] == '\n') {
                ++(exec->program->next_input);
            }
        } else if (exec->input_line == 0) {
            ch = fgetc(exec->input);
            if (ch == '\r') {
                ch = fgetc(exec->input);
                if (ch != '\n' && ch != EOF) {
                    ungetc(ch, exec->input);
                }
            } else if (ch != '\n' && ch != EOF) {
                ungetc(ch, exec->input);
            }
        }
    }

    /* Bail out if an error occurred while reading the input */
    if (status != IP_EXEC_OK) {
        ip_value_release(&value);
        return status;
    }

    /* End of file handling */
    if (eof && exec->at_end_of_input) {
        /* There is an action registered to perform at EOF */
        exec->pc = exec->at_end_of_input;
        ip_value_release(&value);
        return IP_EXEC_OK;
    } else if (eof && exec->pc && exec->pc->type != ITOK_EOL) {
        exec->pc = exec->pc->next;
        ip_value_release(&value);
        return IP_EXEC_OK;
    }

    /* Assign the value to "THIS" */
    ip_value_assign(&(exec->this_value), &value);

    /* Also assign the value to a variable if the destination is not "THIS" */
    if (node->children.left->type != ITOK_THIS) {
        /* Perform the equivalent of "REPLACE var" */
        status = ip_exec_assignment_statement(exec, node);
    }

    /* Clean up and exit */
    ip_value_release(&value);
    return status;
}

/**
 * @brief Writes a value to the output.
 *
 * @param[in,out] exec The execution context.
 * @param[in] node Value to be written, or NULL for "THIS".
 * @param[in] with_eol Non-zero if an EOL should be printed after the value.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_exec_output(ip_exec_t *exec, ip_ast_node_t *node, int with_eol)
{
    ip_value_t value;
    int status = IP_EXEC_OK;
    char buf[64];

    /* Compute the value to be output */
    ip_value_init(&value);
    if (node) {
        status = ip_exec_eval_expression(exec, node, &value);
        if (status != IP_EXEC_OK) {
            ip_value_release(&value);
            return status;
        }
    } else {
        ip_value_assign(&value, &(exec->this_value));
    }

    /* Format the value according to its type.  If we are formatting
     * an implicit "THIS", then use a right-aligned field. */
    switch (value.type) {
    case IP_TYPE_INT:
        if (exec->output_string) {
            if (node) {
                snprintf(buf, sizeof(buf), "%" PRId64, (int64_t)(value.ivalue));
            } else {
                snprintf(buf, sizeof(buf), "%15" PRId64, (int64_t)(value.ivalue));
            }
            (*(exec->output_string))(exec, buf);
        } else if (node) {
            fprintf(exec->output, "%" PRId64, (int64_t)(value.ivalue));
        } else {
            fprintf(exec->output, "%15" PRId64, (int64_t)(value.ivalue));
        }
        break;

    case IP_TYPE_FLOAT:
        if (exec->output_string) {
            if (node) {
                snprintf(buf, sizeof(buf), "%g", value.fvalue);
            } else {
                snprintf(buf, sizeof(buf), "%15.6f", value.fvalue);
            }
            (*(exec->output_string))(exec, buf);
        } else if (node) {
            fprintf(exec->output, "%g", value.fvalue);
        } else {
            fprintf(exec->output, "%15.6f", value.fvalue);
        }
        break;

    case IP_TYPE_STRING:
        if (value.svalue) {
            if (exec->output_string) {
                (*(exec->output_string))(exec, value.svalue->data);
            } else {
                fputs(value.svalue->data, exec->output);
            }
        }
        break;

    default:
        status = IP_EXEC_BAD_TYPE;
        break;
    }

    /* Terminate the value with either a newline or two spaces */
    if (status == IP_EXEC_OK) {
        if (exec->output_char) {
            if (with_eol) {
                (*(exec->output_char))(exec, '\n');
            }
        } else if (with_eol) {
            fputc('\n', exec->output);
        } else if (value.type != IP_TYPE_STRING) {
            fputc(' ', exec->output);
            fputc(' ', exec->output);
        }
    }

    /* Clean up and exit */
    ip_value_release(&value);
    return status;
}

/**
 * @brief Extracts a substring from "THIS".
 *
 * @param[in,out] exec The execution context.
 * @param[in] node The "SUBSTRING FROM" node for the statement.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_exec_extract_substring(ip_exec_t *exec, ip_ast_node_t *node)
{
    ip_value_t str;
    ip_value_t from;
    ip_value_t to;
    ip_string_t *substr;
    int status = IP_EXEC_OK;

    /* Initialise the sub-expressions */
    ip_value_init(&str);
    ip_value_init(&from);
    ip_value_init(&to);

    /* Get the value of "THIS" which must be a string */
    ip_value_assign(&str, &(exec->this_value));
    if (str.type == IP_TYPE_STRING) {
        /* Evaluate the "FROM" and "TO" sub-expressions */
        status = ip_exec_eval_expression(exec, node->children.left, &from);
        if (status == IP_EXEC_OK) {
            status = ip_value_to_int(&from);
        }
        if (status == IP_EXEC_OK) {
            if (node->children.right) {
                status = ip_exec_eval_expression
                    (exec, node->children.right, &to);
                if (status == IP_EXEC_OK) {
                    status = ip_value_to_int(&to);
                }
            } else {
                /* Missing "TO" expression, so use the length of the string */
                ip_value_set_int(&to, str.svalue->len);
            }
        }
        if (status == IP_EXEC_OK) {
            if (from.ivalue < 1 || to.ivalue < 1) {
                /* String indices must be 1 or greater */
                status = IP_EXEC_BAD_INDEX;
                substr = ip_string_create_empty();
            } else if (to.ivalue < from.ivalue) {
                /* Ending index is less than the starting index */
                substr = ip_string_create_empty();
            } else {
                if (to.ivalue > (ip_int_t)(str.svalue->len)) {
                    to.ivalue = (ip_int_t)(str.svalue->len);
                }
                substr = ip_string_substring
                    (str.svalue, from.ivalue - 1, to.ivalue - from.ivalue + 1);
            }
            ip_value_set_string(&(exec->this_value), substr);
            ip_string_deref(substr);
        }
    } else {
        status = IP_EXEC_BAD_TYPE;
    }

    /* Clean up and exit */
    ip_value_release(&str);
    ip_value_release(&from);
    ip_value_release(&to);
    return status;
}

/**
 * @brief Evaluates the arguments to a subroutine call.
 *
 * @param[in,out] exec The execution context.
 * @param[in] frame The new stack frame for passing the arguments.
 * @param[in] arg The node representing the arguments to the "CALL" statement.
 * @param[in,out] num_args Number of arguments in the @a arg list.  Must be
 * zero on first entry.
 *
 * @return IP_EXEC_OK or an error code.
 */
static int ip_exec_eval_call_arguments
    (ip_exec_t *exec, ip_exec_stack_call_t *frame,
     ip_ast_node_t *arg, int *num_args)
{
    int status;
    if (!arg) {
        status = IP_EXEC_OK;
    } else if (arg->type == ITOK_SET) {
        ip_value_t value;
        ip_value_init(&value);
        status = ip_exec_eval_expression(exec, arg->children.right, &value);
        if (status == IP_EXEC_OK) {
            ip_value_assign
                (&(frame->locals[arg->children.left->ivalue]), &value);
        }
        ip_value_release(&value);
        ++(*num_args);
    } else if (arg->type == ITOK_ARG_LIST) {
        status = ip_exec_eval_call_arguments
            (exec, frame, arg->children.left, num_args);
        if (status == IP_EXEC_OK) {
            status = ip_exec_eval_call_arguments
                (exec, frame, arg->children.right, num_args);
        }
    } else {
        status = IP_EXEC_BAD_STATEMENT;
    }
    return status;
}

int ip_exec_step(ip_exec_t *exec)
{
    ip_exec_stack_call_t *frame;
    ip_ast_node_t *node = exec->pc;
    int status;
    int num_args;

    /* Have we fallen off the end of the program? */
    if (!node) {
        /* Set the exit status to 0 and quit the program */
        ip_value_set_int(&(exec->this_value), 0);
        return IP_EXEC_FINISHED;
    }

    /* Remember the location of the node we are about to execute */
    exec->loc = node->loc;

    /* Advance the program counter to the next node.  This may be
     * redirected if we encounter "GO TO", "CALL", "REPEAT", etc. */
    exec->pc = node->next;

    /* Determine what to do based on the next node */
    switch (node->type) {
    case ITOK_LABEL:
    case ITOK_TITLE:
    case ITOK_SYMBOLS_INT:
    case ITOK_MAX_SUBSCRIPTS:
    case ITOK_COMPILE_PROGRAM:
    case ITOK_EOL:
        /* Statement that does nothing at runtime */
        break;

    case ITOK_END_PROGRAM:
        /* Explicit request to end the program */
        exec->pc = node; /* Back up to redo this node if we step again */
        ip_value_set_int(&(exec->this_value), 0);
        return IP_EXEC_FINISHED;

    case ITOK_EXIT_PROGRAM:
        /* Explicit request to end the program with "THIS" as the exit status */
        exec->pc = node; /* Back up to redo this node if we step again */
        return IP_EXEC_FINISHED;

    case ITOK_END_PROCESS:
    case ITOK_RETURN:
        /* Return from a subroutine */
        frame = ip_exec_find_call(exec);
        if (frame) {
            if (node->children.left) {
                /* Evaluate the return expression and copy it into "THIS" */
                status = ip_exec_eval_expression
                    (exec, node->children.left, &(exec->this_value));
                if (status != IP_EXEC_OK) {
                    return status;
                }
            }
            exec->pc = frame->return_node;
            ip_exec_pop_stack_to(exec, frame->base.next);
        } else {
            return IP_EXEC_BAD_RETURN;
        }
        break;

    case ITOK_TAKE:
        /* Take the value of an expression and put the result in "THIS" */
        return ip_exec_eval_expression
            (exec, node->children.left, &(exec->this_value));

    case ITOK_ADD:
    case ITOK_SUBTRACT:
    case ITOK_MULTIPLY:
    case ITOK_DIVIDE:
    case ITOK_MODULO:
    case ITOK_LENGTH_OF:
        /* Perform an arithmetic operation with the result in "THIS" */
        return ip_exec_eval_expression(exec, node, &(exec->this_value));

    case ITOK_REPLACE:
    case ITOK_SET:
        /* Set a variable to an expression value */
        return ip_exec_assignment_statement(exec, node);

    case ITOK_IF:
        /* Conditional statement */
        status = ip_exec_eval_condition(exec, node->children.left);
        if (status == IP_EXEC_FALSE) {
            /* Condition is false, so skip to the next EOL */
            while (exec->pc && exec->pc->type != ITOK_EOL) {
                exec->pc = exec->pc->next;
            }
        } else if (status != IP_EXEC_OK) {
            /* Error occurred while evaluating the condition */
            return status;
        }
        break;

    case ITOK_THEN:
        /* "IF ... THEN ..." structured conditional statement */
        for (;;) {
            exec->pc = node->next;
            status = ip_exec_eval_condition(exec, node->children.left);
            if (status == IP_EXEC_FALSE) {
                /* Condition is false, so skip to the next "ELSE IF",
                 * "ELSE", or "END IF" that matches this "IF" */
                node = exec->pc = node->children.right;
                if (node->type == ITOK_ELSE_IF) {
                    /* Evaluate the condition in the "ELSE IF" clause */
                    continue;
                } else {
                    /* We have hit the "ELSE" or "END IF"; start
                     * execution again from the next statement. */
                    exec->pc = node->next;
                    break;
                }
            } else if (status != IP_EXEC_OK) {
                /* Error occurred while evaluating the condition */
                return status;
            } else {
                /* Condition is true, so continue from the next statement */
                break;
            }
        }
        break;

    case ITOK_ELSE:
    case ITOK_ELSE_IF:
        /* If we encounter an "ELSE" or "ELSE IF", then we have just
         * been executing a previous "THEN" clause.  Skip to "END IF". */
        do {
            exec->pc = node->children.right;
        } while (exec->pc && exec->pc->type != ITOK_END_IF);
        break;

    case ITOK_END_IF:
        /* Nothing to do once we reach the "END IF" */
        break;

    case ITOK_INPUT:
        /* Read a value from the input */
        return ip_exec_input(exec, node);

    case ITOK_PAUSE:
    case ITOK_OUTPUT:
    case ITOK_OUTPUT_NO_EOL:
        /* Write a value to the output */
        return ip_exec_output
            (exec, node->children.left, node->type != ITOK_OUTPUT_NO_EOL);

    case ITOK_GO_TO:
        /* Jump to a specific label */
        return ip_exec_jump_to_label(exec, node->children.left, 0, 0);

    case ITOK_EXECUTE_PROCESS:
    case ITOK_CALL:
        /* Call a subroutine */
        frame = calloc(1, sizeof(ip_exec_stack_call_t));
        if (!frame) {
            ip_out_of_memory();
        }
        frame->base.type = ITOK_CALL;
        frame->return_node = exec->pc;
        num_args = 0;
        if (node->children.right) {
            /* Evaluate the arguments to the call and populate the
             * local variables within the new stack frame. */
            status = ip_exec_eval_call_arguments
                (exec, frame, node->children.right, &num_args);
            if (status != IP_EXEC_OK) {
                ip_exec_free_stack_item(&(frame->base));
                return status;
            }
        }
        frame->base.next = exec->stack;
        exec->stack = &(frame->base);
        return ip_exec_jump_to_label(exec, node->children.left, 1, num_args);

    case ITOK_REPEAT_FROM:
        /* Repeat from a specific label if the loop variable is non-zero */
        return ip_exec_repeat_from(exec, node);

    case ITOK_REPEAT_WHILE:
        /* Repeat while a condition is true */
        return ip_exec_repeat_while(exec, node);

    case ITOK_REPEAT_FOR:
        /* Iterate over a range of values */
        return ip_exec_repeat_for(exec, node);

    case ITOK_END_REPEAT:
        if (node->children.right->type == ITOK_REPEAT_WHILE) {
            /* Jump back to the top of the loop and re-evaluate the condition */
            exec->pc = node->children.right;
        } else {
            /* We are at the end of a "REPEAT FOR".  Find the loop context. */
            ip_exec_stack_loop_t *loop;
            loop = ip_exec_find_loop(exec, node->children.right);
            if (loop) {
                /* Step the loop variable and check for loop termination */
                return ip_exec_repeat_for_next(exec, loop);
            } else {
                /* No matching "REPEAT FOR" for this "END REPEAT" */
                return IP_EXEC_BAD_LOOP;
            }
        }
        break;

    case ITOK_PUNCH:
        /* "PUNCH THE FOLLOWING CHARACTERS" to standard output */
        if (node->text) {
            if (exec->output_string) {
                (*(exec->output_string))(exec, node->text->data);
            } else {
                fputs(node->text->data, exec->output);
            }
        }
        break;

    case ITOK_COPY_TAPE:
        /* Copy the input to the output until the next "~~~~~" or EOF */
        ip_exec_copy_tape(exec, exec->input, 0);
        break;

    case ITOK_IGNORE_TAPE:
        /* Ignore the input up until the next "~~~~~" or EOF */
        ip_exec_copy_tape(exec, exec->input, 1);
        break;

    case ITOK_AT_END_OF_INPUT:
        /* Set the action to take at the end of the input */
        if (!(exec->pc) || exec->pc->type == ITOK_EOL) {
            /* Nothing after this statement, so cancel the EOF handling */
            exec->at_end_of_input = 0;
        } else {
            exec->at_end_of_input = exec->pc;
            while (exec->pc && exec->pc->type != ITOK_EOL) {
                /* Skip the rest of this line */
                exec->pc = exec->pc->next;
            }
        }
        break;

    case ITOK_SUBSTRING:
        return ip_exec_extract_substring(exec, node);

    default:
        /* Unknown statement.  Back up the program counter and exit */
        exec->pc = node;
        return IP_EXEC_BAD_STATEMENT;
    }

    /* If we get here, then the instruction execution was successful */
    return IP_EXEC_OK;
}

int ip_exec_run(ip_exec_t *exec)
{
    const char *error;
    int status;

    /* Keep stepping through the code until finished or error */
    while ((status = ip_exec_step(exec)) == IP_EXEC_OK) {
        /* Do nothing */
    }

    /* If the console is active, deactivate it before printing any errors */
    if (exec->deactivate_console) {
        (*(exec->deactivate_console))(exec);
    }

    /* Determine what happened in the last statement */
    switch (status) {
    case IP_EXEC_FINISHED:
        /* Get the exit status from the final state of the "THIS" variable */
        ip_value_to_int(&(exec->this_value));
        if (exec->this_value.type == IP_TYPE_INT) {
            if (exec->this_value.ivalue >= 0 &&
                    exec->this_value.ivalue <= 255) {
                return (int)(exec->this_value.ivalue);
            }
        }
        return 1; /* "THIS" is not an integer or is out of range */

    case IP_EXEC_DIV_ZERO:      error = "division by zero"; break;
    case IP_EXEC_UNINIT:        error = "uninitialised variable"; break;
    case IP_EXEC_BAD_INDEX:     error = "index out of range"; break;
    case IP_EXEC_BAD_TYPE:      error = "incompatible types"; break;
    case IP_EXEC_BAD_STATEMENT: error = "unknown statement"; break;
    case IP_EXEC_BAD_RETURN:    error = "return from subroutine without call"; break;
    case IP_EXEC_BAD_LABEL:     error = "unknown label"; break;
    case IP_EXEC_BAD_INPUT:     error = "invalid input data"; break;
    case IP_EXEC_BAD_LOCAL:     error = "invalid local variable reference"; break;
    case IP_EXEC_BAD_LOOP:      error = "'END REPEAT' without a matching 'REPEAT'"; break;
    case IP_EXEC_FALSE:         error = "condition is false"; break;
    default:                    error = "program failed"; break;
    }

    /* Print the error and exit */
    if (exec->loc.filename) {
        fprintf(stderr, "%s:", exec->loc.filename);
    }
    fprintf(stderr, "%lu: %s\n", exec->loc.line, error);
    return 2; /* Exit status of 2 for an error */
}
