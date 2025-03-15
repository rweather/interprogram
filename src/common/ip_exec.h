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

#ifndef INTERPROGRAM_EXEC_H
#define INTERPROGRAM_EXEC_H

#include "ip_program.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Status codes for execution functions */
#define IP_EXEC_OK              0   /**< Execution OK, program continuing */
#define IP_EXEC_FINISHED        1   /**< Execution finished successfully */
#define IP_EXEC_DIV_ZERO        2   /**< Division by zero error occurred */
#define IP_EXEC_UNINIT          3   /**< Read from an uninitialised variable */
#define IP_EXEC_BAD_INDEX       4   /**< Array index out of range */
#define IP_EXEC_BAD_TYPE        5   /**< Incompatible types */
#define IP_EXEC_BAD_STATEMENT   6   /**< Unknown statement in program */
#define IP_EXEC_BAD_RETURN      7   /**< Attempt to return from top-level */
#define IP_EXEC_BAD_LABEL       8   /**< Could not find "GO TO" label */
#define IP_EXEC_BAD_INPUT       9   /**< Invalid input data */
#define IP_EXEC_BAD_LOCAL       10  /**< Reference to local at global scope */
#define IP_EXEC_BAD_LOOP        11  /**< "END REPEAT" without matching "FOR" */
#define IP_EXEC_FALSE           12  /**< Condition is false */

/**
 * @brief Value that is "very close to zero" for zero comparisons.
 */
#define IP_FLOAT_EPSILON 1e-20

/**
 * @brief Item on the execution stack for subroutine calls and loops.
 */
typedef struct ip_exec_stack_item_s ip_exec_stack_item_t;
struct ip_exec_stack_item_s
{
    /** Type of stack item: ITOK_CALL or ITOK_REPEAT_FOR */
    int type;

    /** Next lower item on the stack */
    ip_exec_stack_item_t *next;
};

/**
 * @brief Item on the execution stack for subroutine calls.
 */
typedef struct
{
    /** Base class fields */
    ip_exec_stack_item_t base;

    /** Node for control to return back to at the end of the subroutine */
    ip_ast_node_t *return_node;

    /** Local variables for this subroutine */
    ip_value_t locals[IP_MAX_LOCALS];

} ip_exec_stack_call_t;

/**
 * @brief Item on the execution stack for "REPEAT FOR" loops.
 */
typedef struct
{
    /** Base class fields */
    ip_exec_stack_item_t base;

    /** "REPEAT FOR" node for the loop */
    ip_ast_node_t *node;

    /** Points to the iteration variable */
    ip_ast_node_t *var;

    /** End value for the iteration sequence */
    ip_value_t end;

    /** Step value for the iteration sequence */
    ip_value_t step;

} ip_exec_stack_loop_t;

/**
 * @brief Execution context for an INTERPROGRAM.
 */
struct ip_exec_s
{
    /** Points to the parsed program image */
    ip_program_t *program;

    /** The value of the "THIS" variable */
    ip_value_t this_value;

    /** Execution stack for subroutines */
    ip_exec_stack_item_t *stack;

    /** Next node in the program to be executed */
    ip_ast_node_t *pc;

    /** Location of the last node that was executed in the source file */
    ip_loc_t loc;

    /** Stream to read input from (default is stdin) */
    FILE *input;

    /** Stream to write output to (default is stdout) */
    FILE *output;
};

/**
 * @brief Initialises an execution context.
 *
 * @param[out] exec The execution context.
 * @param[in] program Pointer to the program for the execution context.
 *
 * This module takes ownership of @a program and will destroy it
 * when ip_exec_free() is called.
 */
void ip_exec_init(ip_exec_t *exec, ip_program_t *program);

/**
 * @brief Frees an execution context.
 *
 * @param[in] exec The execution context.
 */
void ip_exec_free(ip_exec_t *exec);

/**
 * @brief Resets an execution context.
 *
 * @param[in] exec The execution context.
 *
 * All variables are set back to their defaults and the program counter
 * is set to point at the first statement in the program.
 */
void ip_exec_reset(ip_exec_t *exec);

/**
 * @brief Performs a single instruction.
 *
 * @param[in,out] exec The execution context.
 *
 * @return IP_EXEC_OK if execution is continuing, IP_EXEC_FINISHED
 * if the program finished successfully, or an error code otherwise.
 */
int ip_exec_step(ip_exec_t *exec);

/**
 * @brief Runs the program to completion.
 *
 * @param[in,out] exec The execution context.
 *
 * @return The exit status for the program to return from main().
 */
int ip_exec_run(ip_exec_t *exec);

#ifdef __cplusplus
}
#endif

#endif
