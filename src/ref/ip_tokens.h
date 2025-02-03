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

#ifndef INTERPROGRAM_TOKENS_H
#define INTERPROGRAM_TOKENS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Token codes for the original INTERPROGRAM language */
#define ITOK_BLANK              0x80    /**< Blank separator sequence */
#define ITOK_PRELIM_1           0x81    /**< Preliminary statement (1) */
#define ITOK_PRELIM_2           0x82    /**< Preliminary statement (2) */
#define ITOK_PRELIM_3           0x83    /**< Preliminary statement (3) */
#define ITOK_PRELIM_4           0x84    /**< Preliminary statement (4) */
#define ITOK_END_PRELIM         0x85    /**< End of preliminary statements */
#define ITOK_LABEL              0x86    /**< "*" preceding a GO TO label */
#define ITOK_COMMENT            0x87    /**< "#" that starts a comment */
#define ITOK_AMPERSAND          0x88    /**< "&" to repeat previous statement */
#define ITOK_TITLE              0x89    /**< TITLE */
#define ITOK_SYMBOLS_INT        0x8A    /**< SYMBOLS FOR INTEGERS */
#define ITOK_MAX_SUBSCRIPTS     0x8B    /**< MAXIMUM SUBSCRIPTS */
#define ITOK_COMPILE_PROGRAM    0x8C    /**< COMPILE THE FOLLOWING INTERPROGRAM */
#define ITOK_END_PROGRAM        0x8D    /**< END OF INTERPROGRAM */
#define ITOK_END_PROCESS        0x8E    /**< END OF PROCESS DEFINITION */
#define ITOK_THIS               0x8F    /**< THIS */
#define ITOK_TAKE               0x90    /**< TAKE */
#define ITOK_ADD                0x91    /**< ADD */
#define ITOK_SUBTRACT           0x92    /**< SUBTRACT */
#define ITOK_MULTIPLY           0x93    /**< MULTIPLY BY */
#define ITOK_DIVIDE             0x94    /**< DIVIDE BY */
#define ITOK_IF                 0x95    /**< IF */
#define ITOK_GREATER_THAN       0x96    /**< IS GREATER THAN */
#define ITOK_MUCH_GREATER_THAN  0x97    /**< IS MUCH GREATER THAN */
#define ITOK_SMALLER_THAN       0x98    /**< IS SMALLER THAN */
#define ITOK_MUCH_SMALLER_THAN  0x99    /**< IS MUCH SMALLER THAN */
#define ITOK_ZERO               0x9A    /**< IS ZERO */
#define ITOK_POSITIVE           0x9B    /**< IS POSITIVE */
#define ITOK_NEGATIVE           0x9C    /**< IS NEGATIVE */
#define ITOK_REPLACE            0x9D    /**< REPLACE */
#define ITOK_INPUT              0x9E    /**< INPUT */
#define ITOK_OUTPUT             0x9F    /**< OUTPUT */
#define ITOK_SET                0xA0    /**< SET */
#define ITOK_GO_TO              0xA1    /**< GO TO */
#define ITOK_EXECUTE_PROCESS    0xA2    /**< EXECUTE PROCESS */
#define ITOK_REPEAT_FROM        0xA3    /**< REPEAT FROM */
#define ITOK_TIMES              0xA4    /**< TIMES */
#define ITOK_PAUSE              0xA5    /**< PAUSE */
#define ITOK_PUNCH              0xA6    /**< PUNCH THE FOLLOWING CHARACTERS */
#define ITOK_COPY_TAPE          0xA7    /**< COPY TAPE */
#define ITOK_IGNORE_TAPE        0xA8    /**< IGNORE TAPE */
#define ITOK_SQRT               0xA9    /**< FORM SQUARE ROOT */
#define ITOK_SIN                0xAA    /**< FORM SINE */
#define ITOK_COS                0xAB    /**< FORM COSINE */
#define ITOK_TAN                0xAC    /**< FORM TANGENT */
#define ITOK_ATAN               0xAD    /**< FORM ARCTAN */
#define ITOK_LOG                0xAE    /**< FORM NATURAL LOG */
#define ITOK_EXP                0xAF    /**< FORM EXPONENTIAL */
#define ITOK_NONE               0xB0    /**< FORM EXPONENTIAL */

/* Token codes for extensions to the original INTERPROGRAM language */
#define ITOK_ML_COMMENT         0xB1    /**< Multi-line comment "##" */
#define ITOK_EQUAL_TO           0xB2    /**< IS EQUAL TO */
#define ITOK_NOT_EQUAL_TO       0xB3    /**< IS NOT EQUAL TO */
#define ITOK_ABS                0xB4    /**< FORM ABSOLUTE */
#define ITOK_SIGN               0xB5    /**< FORM SIGN */
#define ITOK_MODULO             0xB6    /**< MODULO */

/* Meta-tokens for non-keyword elements and terminal characters */
#define ITOK_VAR_NAME           0xF0    /**< Variable name */
#define ITOK_INT_VALUE          0xF1    /**< Integer value */
#define ITOK_FLOAT_VALUE        0xF2    /**< Floating-point value */
#define ITOK_ERROR              0xF3    /**< Error token */
#define ITOK_EOF                0x04    /**< End of file encountered */
#define ITOK_EOL                '\n'    /**< End of line marker */
#define ITOK_COMMA              ','     /**< Statement separator */
#define ITOK_LPAREN             '('     /**< Left parenthesis */
#define ITOK_RPAREN             ')'     /**< Right parenthesis */
#define ITOK_EQUAL              '='     /**< Equals sign */

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
    unsigned char code;         /**< Token code */
    unsigned char type;         /**< Type of token */

} ip_token_t;

/**
 * @brief Control structure for tokenising an input stream.
 */
typedef struct
{
    /** Function to read a character from the input stream */
    ip_token_read_char read_char;

    /** User data for the "read_char" function */
    void *user_data;

} ip_tokeniser_t;

#ifdef __cplusplus
}
#endif

#endif
