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

/** Minimum number that can be used for a label */
#define IP_MIN_LABEL_NUMBER 1

/** Maximum number that can be used for a label */
#define IP_MAX_LABEL_NUMBER 9999 /* Classic INTERPROGRAM used 150 */

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

    /** Token code for the last statement, for "&" repetition */
    int last_statement;

    /** Number of errors that were reported during the parsing process */
    unsigned long num_errors;

    /** Number of warnings that were reported during the parsing process */
    unsigned long num_warnings;

    /** Flags that indicate which label numbers are in use */
    unsigned char labels_used[(IP_MAX_LABEL_NUMBER + 8) / 8];

} ip_parser_t;

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
void ip_parse_preliminary_statements(ip_parser_t *parser);

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
