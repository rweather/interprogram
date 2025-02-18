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

#ifndef INTERPROGRAM_TOKEN_H
#define INTERPROGRAM_TOKEN_H

#include "ip_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Token codes for the original INTERPROGRAM language */
#define ITOK_COMMA              0x80    /**< Statement separator */
#define ITOK_LPAREN             0x81    /**< Left parenthesis */
#define ITOK_RPAREN             0x82    /**< Right parenthesis */
#define ITOK_EQUAL              0x83    /**< "=" sign */
#define ITOK_LABEL              0x84    /**< "*" preceding a GO TO label */
#define ITOK_AMPERSAND          0x85    /**< "&" to repeat previous statement */
#define ITOK_PLUS               0x86    /**< "+" sign */
#define ITOK_MINUS              0x87    /**< "-" sign */
#define ITOK_PRELIM_1           0x88    /**< Preliminary statement (1) */
#define ITOK_PRELIM_2           0x89    /**< Preliminary statement (2) */
#define ITOK_PRELIM_3           0x8A    /**< Preliminary statement (3) */
#define ITOK_PRELIM_4           0x8B    /**< Preliminary statement (4) */
#define ITOK_TITLE              0x8C    /**< TITLE */
#define ITOK_SYMBOLS_INT        0x8D    /**< SYMBOLS FOR INTEGERS */
#define ITOK_MAX_SUBSCRIPTS     0x8E    /**< MAXIMUM SUBSCRIPTS */
#define ITOK_COMPILE_PROGRAM    0x8F    /**< COMPILE THE FOLLOWING INTERPROGRAM */
#define ITOK_NONE               0x90    /**< NONE */
#define ITOK_END_PROGRAM        0x91    /**< END OF INTERPROGRAM */
#define ITOK_END_PROCESS        0x92    /**< END OF PROCESS DEFINITION */
#define ITOK_THIS               0x93    /**< THIS */
#define ITOK_TAKE               0x94    /**< TAKE */
#define ITOK_ADD                0x95    /**< ADD */
#define ITOK_SUBTRACT           0x96    /**< SUBTRACT */
#define ITOK_MULTIPLY           0x97    /**< MULTIPLY BY */
#define ITOK_DIVIDE             0x98    /**< DIVIDE BY */
#define ITOK_IF                 0x99    /**< IF */
#define ITOK_GREATER_THAN       0x9A    /**< IS GREATER THAN */
#define ITOK_MUCH_GREATER_THAN  0x9B    /**< IS MUCH GREATER THAN */
#define ITOK_SMALLER_THAN       0x9C    /**< IS SMALLER THAN */
#define ITOK_MUCH_SMALLER_THAN  0x9D    /**< IS MUCH SMALLER THAN */
#define ITOK_ZERO               0x9E    /**< IS ZERO */
#define ITOK_POSITIVE           0x9F    /**< IS POSITIVE */
#define ITOK_NEGATIVE           0xA0    /**< IS NEGATIVE */
#define ITOK_REPLACE            0xA1    /**< REPLACE */
#define ITOK_INPUT              0xA2    /**< INPUT */
#define ITOK_OUTPUT             0xA3    /**< OUTPUT */
#define ITOK_SET                0xA4    /**< SET */
#define ITOK_GO_TO              0xA5    /**< GO TO */
#define ITOK_EXECUTE_PROCESS    0xA6    /**< EXECUTE PROCESS */
#define ITOK_REPEAT_FROM        0xA7    /**< REPEAT FROM */
#define ITOK_TIMES              0xA8    /**< TIMES */
#define ITOK_PAUSE              0xA9    /**< PAUSE */
#define ITOK_PUNCH              0xAA    /**< PUNCH THE FOLLOWING CHARACTERS */
#define ITOK_COPY_TAPE          0xAB    /**< COPY TAPE */
#define ITOK_IGNORE_TAPE        0xAC    /**< IGNORE TAPE */
#define ITOK_SQRT               0xAD    /**< FORM SQUARE ROOT */
#define ITOK_SIN                0xAE    /**< FORM SINE */
#define ITOK_COS                0xAF    /**< FORM COSINE */
#define ITOK_TAN                0xB0    /**< FORM TANGENT */
#define ITOK_ATAN               0xB1    /**< FORM ARCTAN */
#define ITOK_LOG                0xB2    /**< FORM NATURAL LOG */
#define ITOK_EXP                0xB3    /**< FORM EXPONENTIAL */

/* Token codes for extensions to the original INTERPROGRAM language */
#define ITOK_ABS                0xB4    /**< FORM ABSOLUTE */
#define ITOK_EQUAL_TO           0xB5    /**< IS EQUAL TO */
#define ITOK_NOT_EQUAL_TO       0xB6    /**< IS NOT EQUAL TO */
#define ITOK_GREATER_OR_EQUAL   0xB7    /**< IS GREATER THAN OR EQUAL TO */
#define ITOK_SMALLER_OR_EQUAL   0xB8    /**< IS SMALLER THAN OR EQUAL TO */
#define ITOK_NOT_ZERO           0xB9    /**< IS NOT ZERO */
#define ITOK_FINITE             0xBA    /**< IS FINITE */
#define ITOK_INFINITE           0xBB    /**< IS INFINITE */
#define ITOK_NAN                0xBC    /**< IS NOT A NUMBER */
#define ITOK_MODULO             0xBD    /**< MODULO */
#define ITOK_BITWISE_AND_NOT    0xBE    /**< BITWISE AND WITH NOT */
#define ITOK_BITWISE_AND        0xBF    /**< BITWISE AND WITH */
#define ITOK_BITWISE_OR         0xC0    /**< BITWISE OR WITH */
#define ITOK_BITWISE_XOR        0xC1    /**< BITWISE XOR WITH */
#define ITOK_BITWISE_NOT        0xC2    /**< BITWISE NOT */
#define ITOK_SHIFT_LEFT         0xC3    /**< SHIFT LEFT BY */
#define ITOK_SHIFT_RIGHT        0xC4    /**< SHIFT RIGHT BY */
#define ITOK_MUL                0xC5    /**< Multiplication operator "*" */
#define ITOK_DIV                0xC6    /**< Division operator "/" */
#define ITOK_RAISE              0xC7    /**< RAISE TO THE POWER OF */
#define ITOK_CALL               0xC8    /**< CALL */
#define ITOK_RETURN             0xC9    /**< RETURN */

/** First keyword token */
#define ITOK_FIRST_KEYWORD      ITOK_COMMA

/** Last keyword token */
#define ITOK_LAST_KEYWORD       ITOK_RETURN

/* Meta-tokens for non-keyword elements */
#define ITOK_VAR_NAME           0xF0    /**< Variable name */
#define ITOK_INT_VALUE          0xF1    /**< Integer value */
#define ITOK_FLOAT_VALUE        0xF2    /**< Floating-point value */
#define ITOK_ERROR              0xF3    /**< Error token */
#define ITOK_EOF                0xF4    /**< End of file encountered */
#define ITOK_EOL                0xF5    /**< End of line marker */
#define ITOK_TEXT               0xF6    /**< Raw text */
#define ITOK_TO_INT             0xF7    /**< Convert to an integer */
#define ITOK_TO_FLOAT           0xF8    /**< Convert to floating-point */
#define ITOK_TO_DYNAMIC         0xF9    /**< Convert to dynamic */
#define ITOK_INDEX_INT          0xFA    /**< Index into an array of integers */
#define ITOK_INDEX_FLOAT        0xFB    /**< Index into an array of floats */
#define ITOK_OUTPUT_NO_EOL      0xFC    /**< Output with no end of line */

/* Token type flags */
/** Token can start a section of the preliminary statements */
#define ITOK_TYPE_PRELIM_START  0x0001
/** Token can appear in the "(1)" section of the preliminary statements */
#define ITOK_TYPE_PRELIM_1      0x0002
/** Token can appear in the "(2)" section of the preliminary statements */
#define ITOK_TYPE_PRELIM_2      0x0004
/** Token can appear in the "(3)" section of the preliminary statements */
#define ITOK_TYPE_PRELIM_3      0x0008
/** Token can appear in the "(4)" section of the preliminary statements */
#define ITOK_TYPE_PRELIM_4      0x0010
/** Token can introduce a statement */
#define ITOK_TYPE_STATEMENT     0x0020
/** Token can appear as a symbol name in a variable declaration */
#define ITOK_TYPE_SYMBOL_NAME   0x0040
/** Token can appear as a condition in an "IF" statement */
#define ITOK_TYPE_CONDITION     0x0080
/** Token can appear in a "REPEAT" statement */
#define ITOK_TYPE_REPEAT        0x0100
/** Token can appear in a "SET" statement, usually "=" */
#define ITOK_TYPE_SET           0x0200
/** Token can appear in an expression */
#define ITOK_TYPE_EXPRESSION    0x0400
/** Token can appear in a "PUNCH THE FOLLOWING CHARACTERS" context */
#define ITOK_TYPE_PUNCH         0x0800
/** Token follows a minus sign, for recognising negative integer constants */
#define ITOK_TYPE_NEGATIVE      0x1000
/** Token is part of a constant expression (no variables) */
#define ITOK_TYPE_CONSTANT      0x2000
/** Token can appear in any context */
#define ITOK_TYPE_ANY           0x7FFF
/** Token is an extension, not in the original INTERPROGRAM language */
#define ITOK_TYPE_EXTENSION     0x8000

/**
 * @brief Reads a character from an input stream for parsing purposes.
 *
 * @param[in] user_data User data for the input object.
 *
 * @return 0 to 255 for a character, or -1 for the end of the input stream.
 */
typedef int (*ip_token_read_char)(void *user_data);

/**
 * @brief Information about a built-in token (not for names or constants).
 */
typedef struct
{
    const char *name;           /**< Name of the token */
    unsigned short code;        /**< Token code */
    unsigned short flags;       /**< Flags indicating the token type */

} ip_token_info_t;

/**
 * @brief Location of a token in the input stream, for error reporting.
 */
typedef struct
{
    /** Name of the file that contains the token */
    const char *filename;

    /** Number of the line that contains the token */
    unsigned long line;

} ip_loc_t;

/**
 * @brief Control structure for tokenising an input stream.
 */
typedef struct
{
    /* Public fields */

    /** Function to read a character from the input stream */
    ip_token_read_char read_char;

    /** User data for the "read_char" function */
    void *user_data;

    /** Name of the file that is being tokenised */
    const char *filename;

    /** Code for the token that was just recognised */
    int token;

    /** Information about the token that was just recognised */
    const ip_token_info_t *token_info;

    /** Location of the token that was just recognised */
    ip_loc_t loc;

    /** Integer value if the token is ITOK_INT_VALUE */
    ip_uint_t ivalue;

    /** Floating-point value if the token is ITOK_FLOAT_VALUE */
    ip_float_t fvalue;

    /** Number of bits of integer precision between 8 and 64 */
    int integer_precision;

    /* Private fields for internal use only */

    /** Non-zero once we see EOF */
    int saw_eof;

    /** Character that was pushed back into the stream after the last token */
    int unget_char;

    /** Number of the current line */
    unsigned long line;

    /** Buffer for storing the current line temporarily */
    char *buffer;

    /** Position within the current line */
    size_t buffer_posn;

    /** Length of the current line */
    size_t buffer_len;

    /** Maximum length of the current line buffer */
    size_t buffer_max;

    /** Buffer for storing token names temporarily */
    char *name;

    /** Current length of the value in the buffer */
    size_t name_len;

    /** Maximum length of the value in the buffer */
    size_t name_max;

    /** Space for information about variable names and numeric tokens */
    ip_token_info_t token_space;

} ip_tokeniser_t;

/**
 * @brief Initialises a tokeniser for INTERPROGRAM source code input.
 *
 * @param[out] tokeniser The tokeniser to initialise.
 *
 * This function call must be followed by setting "read_char",
 * "user_data", and "filename" in the @a tokeniser object.
 */
void ip_tokeniser_init(ip_tokeniser_t *tokeniser);

/**
 * @brief Frees the memory associated with a tokeniser.
 *
 * @param[in] tokeniser The tokeniser to free.
 */
void ip_tokeniser_free(ip_tokeniser_t *tokeniser);

/**
 * @brief Read the next token from the input stream.
 *
 * @param[in,out] tokeniser The tokeniser to read the next token from.
 * @param[in] context The context in which the token should be parsed;
 * ITOK_TYPE_PRELIM_START, ITOK_TYPE_STATEMENT, etc.
 *
 * @return The token code, which may be ITOK_EOF at the end of the input.
 *
 * If @a context does not include the flag ITOK_TYPE_EXTENSION,
 * then extension keywords are not permitted.
 *
 * More information about the token can be found in the "token_info" field.
 */
int ip_tokeniser_get_next(ip_tokeniser_t *tokeniser, unsigned context);

/**
 * @brief Gets the token information block for a keyword token code.
 *
 * @param[in] token The keyword token code.
 *
 * @return The token information block, or NULL if @a token is not a keyword.
 */
const ip_token_info_t *ip_tokeniser_get_keyword(int token);

/**
 * @brief Look up the token information block for a keyword by name.
 *
 * @param[in] name Points to the name.
 * @param[in] len Length of the name in characters.
 * @param[in] context The context in which the token should be parsed;
 * ITOK_TYPE_PRELIM_START, ITOK_TYPE_STATEMENT, etc.
 *
 * @return The token information block, or NULL if @a name is not a keyword.
 *
 * If @a context does not include the flag ITOK_TYPE_EXTENSION,
 * then extension keywords are not permitted.
 */
const ip_token_info_t *ip_tokeniser_lookup_keyword
    (const char *name, size_t len, unsigned context);

/**
 * @brief Reads the contents of a "PUNCH THE FOLLOWING CHARACTERS" statement.
 *
 * @param[in,out] tokeniser The tokeniser to read the punch text from.
 *
 * @return A string containing all of the characters of the punch text.
 *
 * All characters up to the next sequence of 5 "~" symbols are read,
 * including end of line markers.
 */
char *ip_tokeniser_read_punch(ip_tokeniser_t *tokeniser);

/**
 * @brief Reads the contents of a "TITLE" statement up to the next end of line.
 *
 * @param[in,out] tokeniser The tokeniser to read the title from.
 *
 * @return A string containing all of the characters of the title.
 */
char *ip_tokeniser_read_title(ip_tokeniser_t *tokeniser);

/**
 * @brief Skip the rest of the current line for error recovery purposes.
 *
 * @param[in,out] tokeniser The tokeniser.
 */
void ip_tokeniser_skip_line(ip_tokeniser_t *tokeniser);

#ifdef __cplusplus
}
#endif

#endif
