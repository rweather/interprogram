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

#ifndef INTERPROGRAM_PROGRAM_H
#define INTERPROGRAM_PROGRAM_H

#include "ip_ast.h"
#include "ip_labels.h"
#include "ip_value.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Execution context for an INTERPROGRAM.
 */
typedef struct ip_exec_s ip_exec_t;

/**
 * @brief Function prototype for built-in statements.
 *
 * @param[in,out] exec The execution context.
 * @param[in] args Points to the arguments and local variable space.
 * @param[in] num_args Number of arguments (0 to IP_MAX_LOCALS-1).
 *
 * @return IP_EXEC_OK or an error code.
 *
 * The argument array pointed to by @a args has IP_MAX_LOCALS entries.
 * These can be used for temporary local variable storage by the built-in.
 */
typedef int (*ip_builtin_handler_t)
    (ip_exec_t *exec, ip_value_t *args, size_t num_args);

/**
 * @brief Registered built-in statement in the interpreter.
 */
typedef struct
{
    /** Base class information */
    ip_symbol_t base;

    /** Pointer to the built-in handler */
    ip_builtin_handler_t handler;

} ip_builtin_t;

/**
 * @brief Information about a built-in statement in the interpreter,
 * for registering multiple built-ins in bulk;
 */
typedef struct
{
    /** Name of the built-in, must be in uppercase; NULL at end of the list. */
    const char *name;

    /** Pointer to the built-in handler */
    ip_builtin_handler_t handler;

    /** Minimum number of allowable arguments (0 to 9) */
    signed char min_args;

    /** Maximum number of allowable arguments (0 to 9) */
    signed char max_args;

} ip_builtin_info_t;

/**
 * @brief Structure of a program in memory after it has been parsed.
 */
typedef struct
{
    /** Table containing all variables in the program */
    ip_var_table_t vars;

    /** Table containing all labels in the program */
    ip_label_table_t labels;

    /** List of all statements in the program */
    ip_ast_list_t statements;

    /** Table containing the registered built-in statements */
    ip_symbol_table_t builtins;

    /** Name of the file that the program was loaded from */
    char *filename;

    /** Input data that is embedded in the program */
    char *embedded_input;

    /** Next embedded character to be read by "INPUT" */
    const char *next_input;

} ip_program_t;

/**
 * @brief Creates a new program.
 *
 * @param[in] filename Name of the file the program was loaded from.
 */
ip_program_t *ip_program_new(const char *filename);

/**
 * @brief Frees a program.
 *
 * @param[in] program The program state to free.
 */
void ip_program_free(ip_program_t *program);

/**
 * @brief Resets all variables to zero and marks them as uninitialised.
 *
 * @param[in,out] program The program containing the variables to reset.
 */
void ip_program_reset_variables(ip_program_t *program);

/**
 * @brief Sets the embedded input for the program.
 *
 * @param[in,out] program The program state.
 * @param[in] input The input to be embedded.
 */
void ip_program_set_input(ip_program_t *program, const char *input);

/**
 * @brief Registers a built-in statement with the program.
 *
 * @param[in,out] program The program state.
 * @param[in] name The name of the built-in statement in uppercase.
 * @param[in] handler Handler for the built-in.
 * @param[in] min_args Minimum number of allowable arguments (0 to 9).
 * @param[in] max_args Maximum number of allowable arguments (0 to 9).
 *
 * This function will update the handler and number of arguments if the
 * built-in has already been registered.  This allows pre-defined built-ins
 * to be overridden by the application.
 */
void ip_program_register_builtin
    (ip_program_t *program, const char *name,
     ip_builtin_handler_t handler, signed char min_args,
     signed char max_args);

/**
 * @brief Registers a list of built-in statements with the program.
 *
 * @param[in,out] program The program state.
 * @param[in] builtins Points to the list of built-in statements to register.
 */
void ip_program_register_builtins
    (ip_program_t *program, const ip_builtin_info_t *builtins);

/**
 * @brief Looks up a built-in routine by name.
 *
 * @param[in] program The program state.
 * @param[in] name The name of the built-in statement.
 *
 * @return A pointer to the built-in routine or NULL if not found.
 */
ip_builtin_t *ip_program_lookup_builtin_routine
    (const ip_program_t *program, const char *name);

/**
 * @brief Looks up a built-in function by name.
 *
 * @param[in] program The program state.
 * @param[in] name The name of the built-in statement.
 *
 * @return A pointer to the built-in function or NULL if not found.
 */
ip_builtin_t *ip_program_lookup_builtin_function
    (const ip_program_t *program, const char *name);

/**
 * @brief Validates the number of arguments to a built-in statement.
 *
 * @param[in] builtin The built-in statement.
 * @param[in] num_args The number of arguments to validate.
 *
 * @return Non-zero if @a num_args is in range, or zero otherwise.
 */
int ip_builtin_validate_num_args(ip_builtin_t *builtin, int num_args);

#ifdef __cplusplus
}
#endif

#endif
