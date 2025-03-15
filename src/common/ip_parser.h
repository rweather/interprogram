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

#ifndef INTERPROGRAM_PARSER_H
#define INTERPROGRAM_PARSER_H

#include "ip_program.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Context information for block-structured statements.
 */
typedef struct ip_parse_block_context_s ip_parse_block_context_t;
struct ip_parse_block_context_s
{
    /* Type of block; ITOK_IF, ITOK_REPEAT_WHILE, etc */
    int type;

    /** Pointer to the block's controlling node */
    ip_ast_node_t *control;

    /** Pointer to the node to be back-patched for false conditions */
    ip_ast_node_t *patch;

    /* Next outer block context */
    ip_parse_block_context_t *next;
};

/**
 * @brief Information about an in-progress parse of an INTERPROGRAM program.
 */
typedef struct
{
    /** Tokeniser for reading from the input stream */
    ip_tokeniser_t tokeniser;

    /** Points to the program image that is being built */
    ip_program_t *program;

    /** Flags that control the language dialect; e.g. ITOK_TYPE_EXTENSION */
    unsigned flags;

    /** Inferred type of the "THIS" variable, or IP_TYPE_UNKNOWN */
    unsigned char this_type;

    /** Number of errors that were reported during the parsing process */
    unsigned long num_errors;

    /** Number of warnings that were reported during the parsing process */
    unsigned long num_warnings;

    /** In-progress structured programming blocks */
    ip_parse_block_context_t *blocks;

    /** Pointer to a function handler when the token is ITOK_FUNCTION_NAME */
    ip_builtin_handler_t function_builtin;

} ip_parser_t;

/**
 * @brief Callback function for registering the built-in library.
 *
 * @param[in] parser The parser state.
 * @param[in] options Options for the built-ins to register.
 */
typedef void (*ip_parse_register_builtins_t)
    (ip_parser_t *parser, unsigned options);

/**
 * @brief Initialises a parser state.
 *
 * @param[out] parser The parser state to initialise.
 */
void ip_parse_init(ip_parser_t *parser);

/**
 * @brief Frees a parser state.
 *
 * @param[in] parser The parser state to free.
 */
void ip_parse_free(ip_parser_t *parser);

/**
 * @brief Parses an expression node from the input stream.
 *
 * @param[in,out] parser The parser state.
 *
 * @return The expression node, or NULL if an error occurred.
 *
 * On entry to this function, the tokeniser must be positioned on the
 * first token of the expression.  On exit from this function, the
 * tokeniser will be positioned on the token just after the expression.
 */
ip_ast_node_t *ip_parse_expression(ip_parser_t *parser);

/**
 * @brief Parses a list of statements from the input stream.
 *
 * @param[in,out] parser The parser state.
 *
 * On entry to this function, the tokeniser must be positioned on the
 * first token of the first statement.  On exit from this function, the
 * tokeniser will be positioned at the end of the input stream.
 */
void ip_parse_statements(ip_parser_t *parser);

/**
 * @brief Parses a list of preliminary statements from the input stream.
 *
 * @param[in,out] parser The parser state.
 *
 * On exit from this function, the tokeniser will be positioned at the
 * end of the preliminary statements and the start of the main program code.
 */
void ip_parse_preliminary_statements
    (ip_parser_t *parser, ip_parse_register_builtins_t register_builtins);

/**
 * @brief Checks for undefined labels and prints error messages for them.
 *
 * @param[in,out] parser The parser state.
 */
void ip_parse_check_undefined_labels(ip_parser_t *parser);

/**
 * @brief Checks for open 'IF' and 'REPEAT' blocks and prints error messages.
 *
 * @param[in,out] parser The parser state.
 */
void ip_parse_check_open_blocks(ip_parser_t *parser);

/**
 * @brief Registers built-in statements from the program with the parser.
 *
 * @param[in,out] parser The parser state.
 * @param[in] register_builtins Callback function to register the
 * built-in library when the "TITLE" preliminary statement is encountered.
 */
void ip_parse_register_builtins(ip_parser_t *parser);

/**
 * @brief Parse a program file and return the program image.
 *
 * @param[out] program Points to the program state to load into.
 * @param[in] filename Name of the file to load the program from,
 * or NULL for standard input.
 * @param[in] options Flags for syntax options; e.g. ITOK_TYPE_EXTENSION.
 * @param[in] argc Number of arguments to write into the "ARGV" variable,
 * or zero for no "ARGV" variable.
 * @param[in] argv Array of arguments for the "ARGV" variable.
 * @param[in] register_builtins Callback function to register the
 * built-in library.
 *
 * @return Zero on success or the number of errors that occured.
 */
unsigned long ip_parse_program_file
    (ip_program_t *program, const char *filename, unsigned options,
     int argc, char **argv, ip_parse_register_builtins_t register_builtins);

/**
 * @brief Prints an error message for the current line.
 *
 * @param[in,out] parser The parser state.
 * @param[in] format The printf-style format for the error message.
 */
void ip_error(ip_parser_t *parser, const char *format, ...);

/**
 * @brief Prints an error message at a specific location
 *
 * @param[in,out] parser The parser state.
 * @param[in] loc The location of the error.
 * @param[in] format The printf-style format for the error message.
 */
void ip_error_at
    (ip_parser_t *parser, const ip_loc_t *loc, const char *format, ...);

/**
 * @brief Prints an error message "at or near" the current token.
 *
 * @param[in,out] parser The parser state.
 * @param[in] format The printf-style format for the error message.
 */
void ip_error_near(ip_parser_t *parser, const char *format, ...);

/**
 * @brief Prints a warning message for the current line.
 *
 * @param[in,out] parser The parser state.
 * @param[in] format The printf-style format for the warning message.
 */
void ip_warning(ip_parser_t *parser, const char *format, ...);

/**
 * @brief Prints a warning message at a specific location.
 *
 * @param[in,out] parser The parser state.
 * @param[in] loc The location of the warning.
 * @param[in] format The printf-style format for the warning message.
 */
void ip_warning_at
    (ip_parser_t *parser, const ip_loc_t *loc, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
