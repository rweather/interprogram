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

#include "ip_token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ip_token_info_t const tokens[] = {
    {",",                                   ITOK_COMMA,             ITOK_TYPE_ANY},
    {"(",                                   ITOK_LPAREN,            ITOK_TYPE_ANY},
    {")",                                   ITOK_RPAREN,            ITOK_TYPE_ANY},
    {"=",                                   ITOK_EQUAL,             ITOK_TYPE_SET},
    {"*",                                   ITOK_LABEL,             ITOK_TYPE_STATEMENT | ITOK_TYPE_REPEAT},
    {"&",                                   ITOK_AMPERSAND,         ITOK_TYPE_STATEMENT},
    {"+",                                   ITOK_PLUS,              ITOK_TYPE_EXPRESSION},
    {"-",                                   ITOK_MINUS,             ITOK_TYPE_EXPRESSION},
    {"(1)",                                 ITOK_PRELIM_1,          ITOK_TYPE_PRELIM_START},
    {"(2)",                                 ITOK_PRELIM_2,          ITOK_TYPE_PRELIM_START},
    {"(3)",                                 ITOK_PRELIM_3,          ITOK_TYPE_PRELIM_START},
    {"(4)",                                 ITOK_PRELIM_4,          ITOK_TYPE_PRELIM_START},
    {"TITLE",                               ITOK_TITLE,             ITOK_TYPE_PRELIM_1},
    {"SYMBOLS FOR INTEGERS",                ITOK_SYMBOLS_INT,       ITOK_TYPE_PRELIM_2},
    {"MAXIMUM SUBSCRIPTS",                  ITOK_MAX_SUBSCRIPTS,    ITOK_TYPE_PRELIM_3},
    {"COMPILE THE FOLLOWING INTERPROGRAM",  ITOK_COMPILE_PROGRAM,   ITOK_TYPE_PRELIM_4},
    {"NONE",                                ITOK_NONE,              ITOK_TYPE_SYMBOL_NAME},
    {"END OF INTERPROGRAM",                 ITOK_END_PROGRAM,       ITOK_TYPE_STATEMENT},
    {"END OF PROCESS DEFINITION",           ITOK_END_PROCESS,       ITOK_TYPE_STATEMENT},
    {"THIS",                                ITOK_THIS,              ITOK_TYPE_EXPRESSION},
    {"TAKE",                                ITOK_TAKE,              ITOK_TYPE_STATEMENT},
    {"ADD",                                 ITOK_ADD,               ITOK_TYPE_STATEMENT},
    {"SUBTRACT",                            ITOK_SUBTRACT,          ITOK_TYPE_STATEMENT},
    {"MULTIPLY BY",                         ITOK_MULTIPLY,          ITOK_TYPE_STATEMENT},
    {"DIVIDE BY",                           ITOK_DIVIDE,            ITOK_TYPE_STATEMENT},
    {"IF",                                  ITOK_IF,                ITOK_TYPE_STATEMENT},
    {"IS GREATER THAN",                     ITOK_GREATER_THAN,      ITOK_TYPE_CONDITION},
    {"IS MUCH GREATER THAN",                ITOK_MUCH_GREATER_THAN, ITOK_TYPE_CONDITION},
    {"IS SMALLER THAN",                     ITOK_SMALLER_THAN,      ITOK_TYPE_CONDITION},
    {"IS MUCH SMALLER THAN",                ITOK_MUCH_SMALLER_THAN, ITOK_TYPE_CONDITION},
    {"IS ZERO",                             ITOK_ZERO,              ITOK_TYPE_CONDITION},
    {"IS POSITIVE",                         ITOK_POSITIVE,          ITOK_TYPE_CONDITION},
    {"IS NEGATIVE",                         ITOK_NEGATIVE,          ITOK_TYPE_CONDITION},
    {"REPLACE",                             ITOK_REPLACE,           ITOK_TYPE_STATEMENT},
    {"INPUT",                               ITOK_INPUT,             ITOK_TYPE_STATEMENT},
    {"OUTPUT",                              ITOK_OUTPUT,            ITOK_TYPE_STATEMENT},
    {"SET",                                 ITOK_SET,               ITOK_TYPE_STATEMENT},
    {"GO TO",                               ITOK_GO_TO,             ITOK_TYPE_STATEMENT},
    {"EXECUTE PROCESS",                     ITOK_EXECUTE_PROCESS,   ITOK_TYPE_STATEMENT},
    {"REPEAT FROM",                         ITOK_REPEAT_FROM,       ITOK_TYPE_STATEMENT},
    {"TIMES",                               ITOK_TIMES,             ITOK_TYPE_REPEAT},
    {"PAUSE",                               ITOK_PAUSE,             ITOK_TYPE_STATEMENT},
    {"PUNCH THE FOLLOWING CHARACTERS",      ITOK_PUNCH,             ITOK_TYPE_STATEMENT},
    {"COPY TAPE",                           ITOK_COPY_TAPE,         ITOK_TYPE_STATEMENT},
    {"IGNORE TAPE",                         ITOK_IGNORE_TAPE,       ITOK_TYPE_STATEMENT},
    {"FORM SQUARE ROOT",                    ITOK_SQRT,              ITOK_TYPE_STATEMENT},
    {"FORM SINE",                           ITOK_SIN,               ITOK_TYPE_STATEMENT},
    {"FORM COSINE",                         ITOK_COS,               ITOK_TYPE_STATEMENT},
    {"FORM TANGENT",                        ITOK_TAN,               ITOK_TYPE_STATEMENT},
    {"FORM ARCTAN",                         ITOK_ATAN,              ITOK_TYPE_STATEMENT},
    {"FORM NATURAL LOG",                    ITOK_LOG,               ITOK_TYPE_STATEMENT},
    {"FORM EXPONENTIAL",                    ITOK_EXP,               ITOK_TYPE_STATEMENT},
    {"FORM ABSOLUTE",                       ITOK_ABS,               ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"IS EQUAL TO",                         ITOK_EQUAL_TO,          ITOK_TYPE_CONDITION | ITOK_TYPE_EXTENSION},
    {"IS NOT EQUAL TO",                     ITOK_NOT_EQUAL_TO,      ITOK_TYPE_CONDITION | ITOK_TYPE_EXTENSION},
    {"IS NOT ZERO",                         ITOK_NOT_ZERO,          ITOK_TYPE_CONDITION | ITOK_TYPE_EXTENSION},
    {"IS FINITE",                           ITOK_FINITE,            ITOK_TYPE_CONDITION | ITOK_TYPE_EXTENSION},
    {"IS INFINITE",                         ITOK_INFINITE,          ITOK_TYPE_CONDITION | ITOK_TYPE_EXTENSION},
    {"IS NOT A NUMBER",                     ITOK_NAN,               ITOK_TYPE_CONDITION | ITOK_TYPE_EXTENSION},
    {"SUBTRACT FROM",                       ITOK_SUBTRACT_FROM,     ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"DIVIDE INTO",                         ITOK_DIVIDE_INTO,       ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"MODULO",                              ITOK_MODULO,            ITOK_TYPE_STATEMENT | ITOK_TYPE_EXPRESSION | ITOK_TYPE_EXTENSION},
    {"BITWISE AND WITH NOT",                ITOK_BITWISE_AND_NOT,   ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"BITWISE AND WITH",                    ITOK_BITWISE_AND,       ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"BITWISE OR WITH",                     ITOK_BITWISE_OR,        ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"BITWISE XOR WITH",                    ITOK_BITWISE_XOR,       ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"BITWISE NOT",                         ITOK_BITWISE_NOT,       ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"SHIFT LEFT BY",                       ITOK_SHIFT_LEFT,        ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"SHIFT RIGHT BY",                      ITOK_SHIFT_RIGHT,       ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"*",                                   ITOK_MUL,               ITOK_TYPE_EXPRESSION | ITOK_TYPE_EXTENSION},
    {"/",                                   ITOK_DIV,               ITOK_TYPE_EXPRESSION | ITOK_TYPE_EXTENSION},
    {"RAISE TO THE POWER OF",               ITOK_RAISE,             ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {0,                                     ITOK_ERROR,             0}
};

/**
 * @brief Adds a character to the tokeniser's line buffer.
 *
 * @param[in,out] tokeniser The tokeniser.
 * @param[in] ch The character to add.
 */
static void ip_tokeniser_add_line(ip_tokeniser_t *tokeniser, int ch)
{
    if (tokeniser->buffer_len >= tokeniser->buffer_max) {
        tokeniser->buffer_max = tokeniser->buffer_len + 1024;
        tokeniser->buffer = realloc(tokeniser->buffer, tokeniser->buffer_max);
        if (!(tokeniser->buffer)) {
            fputs("out of memory\n", stderr);
            exit(2);
        }
    }
    tokeniser->buffer[(tokeniser->buffer_len)++] = (char)ch;
}

/**
 * @brief Adds a character to the tokeniser's name buffer.
 *
 * @param[in,out] tokeniser The tokeniser.
 * @param[in] ch The character to add.
 */
static void ip_tokeniser_add_name(ip_tokeniser_t *tokeniser, int ch)
{
    if (tokeniser->name_len >= tokeniser->name_max) {
        tokeniser->name_max = tokeniser->name_len + 1024;
        tokeniser->name = realloc(tokeniser->name, tokeniser->name_max);
        if (!(tokeniser->name)) {
            fputs("out of memory\n", stderr);
            exit(2);
        }
    }
    tokeniser->name[(tokeniser->name_len)++] = (char)ch;
}

/**
 * @brief Adds a sequence of characters to the tokeniser's name buffer.
 *
 * @param[in,out] tokeniser The tokeniser.
 * @param[in] name Points to the name characters to add.
 * @param[in] len Number of name characters to add.
 */
static void ip_tokeniser_add_name_chars
    (ip_tokeniser_t *tokeniser, const char *name, size_t len)
{
    while (len > 0) {
        ip_tokeniser_add_name(tokeniser, *name++);
        --len;
    }
}

/**
 * @brief Set a token with the name coming from the tokeniser's name buffer.
 *
 * @param[in,out] tokeniser The tokeniser.
 * @param[in] token The token code.
 */
static void ip_tokeniser_set_token(ip_tokeniser_t *tokeniser, int token)
{
    ip_tokeniser_add_name(tokeniser, '\0');
    tokeniser->token = token;
    tokeniser->token_info = &(tokeniser->token_space);
    tokeniser->token_space.name = tokeniser->name;
    tokeniser->token_space.code = token;
    tokeniser->token_space.flags = 0;
    tokeniser->loc.filename = tokeniser->filename;
    tokeniser->loc.line = tokeniser->line;
}

/**
 * @brief Set a token with a specific keyword information block.
 *
 * @param[in,out] tokeniser The tokeniser.
 * @param[in] info The token information block.
 */
static void ip_tokeniser_set_token_info
    (ip_tokeniser_t *tokeniser, const ip_token_info_t *info)
{
    tokeniser->token = info->code;
    tokeniser->token_info = info;
    tokeniser->loc.filename = tokeniser->filename;
    tokeniser->loc.line = tokeniser->line;
}

/**
 * @brief Set a token with a specific keyword code.
 *
 * @param[in,out] tokeniser The tokeniser.
 * @param[in] token The token code.
 */
static void ip_tokeniser_set_token_code(ip_tokeniser_t *tokeniser, int token)
{
    ip_tokeniser_set_token_info
        (tokeniser, &(tokens[token - ITOK_FIRST_KEYWORD]));
}

static int ip_tokeniser_read_default(void *user_data)
{
    (void)user_data;
    return -1;
}

void ip_tokeniser_init(ip_tokeniser_t *tokeniser)
{
    memset(tokeniser, 0, sizeof(ip_tokeniser_t));
    tokeniser->read_char = ip_tokeniser_read_default;
    tokeniser->filename = "<none>";
    tokeniser->unget_char = -1;
    tokeniser->integer_precision = 64;
    ip_tokeniser_set_token(tokeniser, ITOK_ERROR);
}

void ip_tokeniser_free(ip_tokeniser_t *tokeniser)
{
    if (tokeniser->buffer) {
        free(tokeniser->buffer);
    }
    if (tokeniser->name) {
        free(tokeniser->name);
    }
    memset(tokeniser, 0, sizeof(ip_tokeniser_t));
}

/**
 * @brief Determine if a character is non-EOL whitespace.
 *
 * @param[in] ch The character to test.
 * @param[in] context The lexical context for the character.
 *
 * @return Non-zero if @a ch is whitespace; zero if not.
 */
static int ip_tokeniser_is_space(int ch)
{
    if (ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f') {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief Determine if a character is alphabetic.
 *
 * @param[in] ch The character to check.
 *
 * @return Non-zero if the character is alphabetic; zero if not.
 */
static int ip_tokeniser_is_alpha(int ch)
{
    if (ch >= 'A' && ch <= 'Z') {
        return 1;
    } else if (ch >= 'a' && ch <= 'z') {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief Gets a number from the input and detects if integer or floating-point.
 *
 * @param[in,out] tokeniser The tokeniser to get the number from.
 * @param[in] context The parsing context for recognising keywords.
 */
static void ip_tokeniser_get_number(ip_tokeniser_t *tokeniser, unsigned context)
{
    int is_float = 0;
    int float_error = 0;
    int have_digit;
    int exp_is_e;
    size_t posn;
    uint64_t limit;
    int ch;

    /* Recognise the rest of the whole number part */
    while (tokeniser->buffer_posn < tokeniser->buffer_len) {
        ch = tokeniser->buffer[tokeniser->buffer_posn];
        if (ch >= '0' && ch <= '9') {
            ip_tokeniser_add_name(tokeniser, ch);
            ++(tokeniser->buffer_posn);
        } else {
            break;
        }
    }

    /* Check for floating-point lexical tokens */
    if (tokeniser->buffer_posn < tokeniser->buffer_len) {
        ch = tokeniser->buffer[tokeniser->buffer_posn];
        if (ch == '.') {
            is_float = 1;
            ip_tokeniser_add_name(tokeniser, ch);
            ++(tokeniser->buffer_posn);
            have_digit = 0;
            while (tokeniser->buffer_posn < tokeniser->buffer_len) {
                ch = tokeniser->buffer[tokeniser->buffer_posn];
                if (ch >= '0' && ch <= '9') {
                    ip_tokeniser_add_name(tokeniser, ch);
                    ++(tokeniser->buffer_posn);
                    have_digit = 1;
                } else {
                    break;
                }
            }
            if (!have_digit) {
                /* Make sure the mantissa of the float ends with ".0" */
                ip_tokeniser_add_name(tokeniser, '0');
            }
        }
    }
    if (tokeniser->buffer_posn < tokeniser->buffer_len) {
        /* Traditional INTERPROGRAM uses notation like "1.5(17)" to
         * represent exponents.  If extension mode is enabled, then
         * we also allow the modern "1.5e17" notation to be used. */
        ch = tokeniser->buffer[tokeniser->buffer_posn];
        if ((ch == '(') || ((ch == 'e' || ch == 'E') &&
                            (context & ITOK_TYPE_EXTENSION) != 0)) {
            exp_is_e = (ch != '(');
            ++(tokeniser->buffer_posn);
            is_float = 1;
            ip_tokeniser_add_name(tokeniser, 'E');
            if (tokeniser->buffer_posn < tokeniser->buffer_len) {
                ch = tokeniser->buffer[tokeniser->buffer_posn];
                if (ch == '-') {
                    ip_tokeniser_add_name(tokeniser, '-');
                    ++(tokeniser->buffer_posn);
                } else if (ch == '+') {
                    ++(tokeniser->buffer_posn);
                }
            }
            have_digit = 0;
            while (tokeniser->buffer_posn < tokeniser->buffer_len) {
                ch = tokeniser->buffer[tokeniser->buffer_posn];
                if (ch >= '0' && ch <= '9') {
                    ip_tokeniser_add_name(tokeniser, ch);
                    ++(tokeniser->buffer_posn);
                    have_digit = 1;
                } else {
                    break;
                }
            }
            if (!have_digit) {
                /* We are missing the digits of the exponent */
                float_error = 1;
            }
            if (!exp_is_e) {
                /* We expect a closing ")" for the exponent */
                if (tokeniser->buffer_posn < tokeniser->buffer_len) {
                    ch = tokeniser->buffer[tokeniser->buffer_posn];
                    if (ch == ')') {
                        ++(tokeniser->buffer_posn);
                    } else {
                        float_error = 1;
                    }
                } else {
                    float_error = 1;
                }
            }
        }
    }

    /* Bail out if there is a floating-point error */
    if (float_error) {
        ip_tokeniser_set_token(tokeniser, ITOK_ERROR);
        return;
    }

    /* Determine the limit of integer values */
    limit = 1ULL << (tokeniser->integer_precision - 1);
    if ((context & ITOK_TYPE_NEGATIVE) == 0) {
        --limit;
    }

    /* We are done if we already know if the value is floating-point.
     * It is possible that integer values may be out of range for the
     * integer precision.  In that case we switch to floating-point. */
    if (!is_float && tokeniser->name_len <= 19) {
        tokeniser->ivalue = 0;
        for (posn = 0; posn < tokeniser->name_len; ++posn) {
            if (tokeniser->ivalue >= 10000000000000000000ULL) {
                break;
            }
            tokeniser->ivalue *= 10;
            tokeniser->ivalue += (unsigned)(tokeniser->name[posn] - '0');
        }
        if (posn >= tokeniser->name_len && tokeniser->ivalue <= limit) {
            ip_tokeniser_set_token(tokeniser, ITOK_INT_VALUE);
            return;
        }
    }

    /* Convert the number to floating-point and return */
    ip_tokeniser_add_name(tokeniser, '\0');
    tokeniser->ivalue = 0;
    tokeniser->fvalue = strtod(tokeniser->name, NULL);
    ip_tokeniser_set_token(tokeniser, ITOK_FLOAT_VALUE);
}

/**
 * @brief Gets an identifier and converts it into a keyword or variable name.
 *
 * @param[in,out] tokeniser The tokeniser to get the identifier from.
 * @param[in] context The parsing context for recognising keywords.
 */
static void ip_tokeniser_get_identifier
    (ip_tokeniser_t *tokeniser, unsigned context)
{
    const ip_token_info_t *info;
    size_t posn = tokeniser->buffer_posn - 1;
    size_t end_var = posn;
    int ch;
    int prefix = 0;
    int prefix_next = 0;
    for (;;) {
        /* Skip forward to the next non-alphabetic character */
        while (tokeniser->buffer_posn < tokeniser->buffer_len) {
            ch = tokeniser->buffer[tokeniser->buffer_posn];
            if (!ip_tokeniser_is_alpha(ch)) {
                break;
            }
            ++(tokeniser->buffer_posn);
        }

        /* First word will be a variable if we don't find a keyword */
        if (end_var == posn) {
            end_var = tokeniser->buffer_posn;
        }

        /* Do we have a full keyword? */
        info = ip_tokeniser_lookup_keyword
            (tokeniser->buffer + posn, tokeniser->buffer_posn - posn,
             context, &prefix_next);
        if (info) {
            /* Full keyword found */
            ip_tokeniser_set_token_info(tokeniser, info);
            return;
        }

        /* Keep track of whether we have a prefix on the leading words.
         * This prevents the use of the first word as a variable.
         * For example, "FORM" cannot be used as a variable because it is a
         * prefix of "FORM SQUARE ROOT", "FORM SINE", etc. */
        prefix |= prefix_next;

        /* If there is whitespace followed by another word, then try
         * adding another word to the in-progress keyword */
        if (tokeniser->buffer_posn < tokeniser->buffer_len) {
            ch = tokeniser->buffer[tokeniser->buffer_posn];
            if (ch != '\n' && ip_tokeniser_is_space(ch)) {
                ++(tokeniser->buffer_posn);
                while (tokeniser->buffer_posn < tokeniser->buffer_len) {
                    ch = tokeniser->buffer[tokeniser->buffer_posn];
                    if (ch == '\n' || !ip_tokeniser_is_space(ch)) {
                        break;
                    }
                    ++(tokeniser->buffer_posn);
                }
                if (tokeniser->buffer_posn < tokeniser->buffer_len) {
                    ch = tokeniser->buffer[tokeniser->buffer_posn];
                    if (ip_tokeniser_is_alpha(ch)) {
                        continue;
                    }
                }
            }
        }
        break;
    }
    tokeniser->buffer_posn = end_var;
    tokeniser->name_len = 0;
    ip_tokeniser_add_name_chars
        (tokeniser, tokeniser->buffer + posn, tokeniser->buffer_posn - posn);
    if (prefix) {
        /* First word cannot be a variable name as it is a prefix */
        ip_tokeniser_set_token(tokeniser, ITOK_ERROR);
    } else {
        /* First word is not a prefix, so it can be a variable name */
        for (posn = 0; posn < tokeniser->name_len; ++posn) {
            /* Convert the variable name to upper case */
            ch = tokeniser->name[posn];
            if (ch >= 'a' && ch <= 'z') {
                ch = ch - 'a' + 'A';
                tokeniser->name[posn] = (char)ch;
            }
        }
        ip_tokeniser_set_token(tokeniser, ITOK_VAR_NAME);
    }
}

int ip_tokeniser_get_next(ip_tokeniser_t *tokeniser, unsigned context)
{
    int ch;

    /* Clear the token name and value */
    tokeniser->name_len = 0;
    tokeniser->ivalue = 0;
    tokeniser->fvalue = 0;

    /* Do we need a new line of input? */
    if (tokeniser->buffer_posn >= tokeniser->buffer_len) {
        if (tokeniser->saw_eof) {
            /* We already saw EOF last time, so we are done */
            ip_tokeniser_set_token(tokeniser, ITOK_EOF);
            return ITOK_EOF;
        }
        ++(tokeniser->line);
        tokeniser->buffer_posn = 0;
        tokeniser->buffer_len = 0;
        for (;;) {
            if (tokeniser->unget_char == -1) {
                ch = (*(tokeniser->read_char))(tokeniser->user_data);
            } else {
                ch = tokeniser->unget_char;
                tokeniser->unget_char = -1;
            }
            if (ch == '\n') {
                /* LF on its own marks the end of the line */
                ip_tokeniser_add_line(tokeniser, '\n');
                break;
            } else if (ch == '\r') {
                /* CR or CRLF also marks the end of the line */
                ip_tokeniser_add_line(tokeniser, '\n');
                ch = (*(tokeniser->read_char))(tokeniser->user_data);
                if (ch == -1) {
                    /* CR followed by EOF - remember the EOF */
                    tokeniser->saw_eof = 1;
                    break;
                } else if (ch != '\n') {
                    /* CR not followed by LF - save ch for next time */
                    tokeniser->unget_char = ch;
                }
                break;
            } else if (ch == -1) {
                tokeniser->saw_eof = 1;
                if (tokeniser->buffer_len == 0) {
                    /* EOF right at the start of the line */
                    ip_tokeniser_set_token(tokeniser, ITOK_EOF);
                    return ITOK_EOF;
                }

                /* Last line before EOF is missing EOL, so add it */
                ip_tokeniser_add_line(tokeniser, '\n');
            } else {
                ip_tokeniser_add_line(tokeniser, ch);
            }
        }
    }

    /* Skip whitespace before the next token */
    while (tokeniser->buffer_posn < tokeniser->buffer_len) {
        ch = tokeniser->buffer[tokeniser->buffer_posn];
        if (ip_tokeniser_is_space(ch) || ch == '\n') {
            ++(tokeniser->buffer_posn);
        } else {
            break;
        }
    }

    /* Bail out if we are now at the end of the line */
    if (tokeniser->buffer_posn >= tokeniser->buffer_len) {
        ip_tokeniser_set_token(tokeniser, ITOK_EOL);
        return ITOK_EOL;
    }

#define IP_SIMPLE_TOKEN(ch, token, type) \
    case (ch): \
        if (((type) & ITOK_TYPE_EXTENSION) != 0 && \
                (context & (type)) == (type)) { \
            ip_tokeniser_set_token_code(tokeniser, (token)); \
        } else if ((context & (type)) != 0) { \
            ip_tokeniser_set_token_code(tokeniser, (token)); \
        } else { \
            ip_tokeniser_set_token(tokeniser, ITOK_ERROR); \
        } \
        break;

    /* Determine what to do based on the next character */
    ch = tokeniser->buffer[(tokeniser->buffer_posn)++];
    ip_tokeniser_add_name(tokeniser, ch);
    switch (ch) {
    case '#':
        /* We have a comment, so convert it into an EOL marker with the
         * name of the token set to the contents of the comment.  The parser
         * can choose to store the comment or discard it entirely. */
        tokeniser->name_len = 0; /* Drop the '#' from the token name */
        while (tokeniser->buffer_posn < tokeniser->buffer_len &&
                    tokeniser->buffer[tokeniser->buffer_posn] != '\n') {
            ch = tokeniser->buffer[(tokeniser->buffer_posn)++];
            ip_tokeniser_add_name(tokeniser, ch);
        }
        tokeniser->buffer_posn = tokeniser->buffer_len;
        ip_tokeniser_set_token(tokeniser, ITOK_EOL);
        break;

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        ip_tokeniser_get_number(tokeniser, context);
        break;

    case 'A': case 'B': case 'C': case 'D': case 'E':
    case 'F': case 'G': case 'H': case 'I': case 'J':
    case 'K': case 'L': case 'M': case 'N': case 'O':
    case 'P': case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X': case 'Y':
    case 'Z':
    case 'a': case 'b': case 'c': case 'd': case 'e':
    case 'f': case 'g': case 'h': case 'i': case 'j':
    case 'k': case 'l': case 'm': case 'n': case 'o':
    case 'p': case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x': case 'y':
    case 'z':
        ip_tokeniser_get_identifier(tokeniser, context);
        break;

    /* Simple single-character tokens made up of punctuation characters */
    IP_SIMPLE_TOKEN(',', ITOK_COMMA,        ITOK_TYPE_ANY)
    IP_SIMPLE_TOKEN(')', ITOK_RPAREN,       ITOK_TYPE_ANY)
    IP_SIMPLE_TOKEN('=', ITOK_EQUAL,        ITOK_TYPE_SET)
    IP_SIMPLE_TOKEN('&', ITOK_AMPERSAND,    ITOK_TYPE_STATEMENT)
    IP_SIMPLE_TOKEN('+', ITOK_PLUS,         ITOK_TYPE_EXPRESSION)
    IP_SIMPLE_TOKEN('-', ITOK_MINUS,        ITOK_TYPE_EXPRESSION)
    IP_SIMPLE_TOKEN('/', ITOK_DIV,          ITOK_TYPE_EXPRESSION | ITOK_TYPE_EXTENSION)

    case '(':
        /* Special handling for '(' in preliminary sections, for the
         * cases of the "(1)", "(2)", "(3)", and "(4)" prefixes */
        if ((context & ITOK_TYPE_PRELIM_START) != 0) {
            if ((tokeniser->buffer_posn + 2) <= tokeniser->buffer_len &&
                    tokeniser->buffer[tokeniser->buffer_posn + 1] == ')') {
                ch = tokeniser->buffer[tokeniser->buffer_posn];
                if (ch >= '1' && ch <= '4') {
                    tokeniser->buffer_posn += 2;
                    ip_tokeniser_set_token_code
                        (tokeniser, ITOK_PRELIM_1 + ch - '1');
                    break;
                }
            }
        }
        ip_tokeniser_set_token_code(tokeniser, ITOK_LPAREN);
        break;

    case '*':
        /* Special handling for '*' is needed in extended expression mode */
        if ((context & (ITOK_TYPE_EXPRESSION | ITOK_TYPE_EXTENSION)) ==
                (ITOK_TYPE_EXPRESSION | ITOK_TYPE_EXTENSION)) {
            ip_tokeniser_set_token_code(tokeniser, ITOK_MUL);
        } else {
            ip_tokeniser_set_token_code(tokeniser, ITOK_LABEL);
        }
        break;

    default:
        /* Unknown character, so report it as an error */
        ip_tokeniser_set_token(tokeniser, ITOK_ERROR);
        break;
    }

    /* Return the token to the caller */
    return tokeniser->token;
}

const ip_token_info_t *ip_tokeniser_get_keyword(int token)
{
    if (token < ITOK_FIRST_KEYWORD || token > ITOK_LAST_KEYWORD) {
        return 0;
    }
    return &(tokens[token - ITOK_FIRST_KEYWORD]);
}

/**
 * @brief Match an alphabetic keyword token.
 *
 * @param[in] name Points to the name to match.
 * @param[in] len Length of the name in characters.
 * @param[in] name2 Points to the normalised keyword name to match against.
 * @param[out] prefix Set to non-zero on output if @a name is not a
 * keyword but it is a prefix for a keyword so it cannot be an identifier.
 *
 * @return Non-zero if the keyword matches, zero if not.
 */
static int ip_tokeniser_match_keyword
    (const char *name, size_t len, const char *name2, int *prefix)
{
    size_t posn = 0;
    while (len > 0 && *name2 != '\0') {
        if (*name2 == ' ') {
            /* We expect one or more whitespace characters in this position */
            if (!ip_tokeniser_is_space(*name)) {
                return 0;
            }
            while (len > 0 && ip_tokeniser_is_space(*name)) {
                ++name;
                --len;
            }
            if (posn != 0) {
                /* We have already matched a keyword prefix, which means
                 * that the preceding words cannot be a variable name */
                *prefix = 1;
            }
        } else {
            /* Match the next alphabetic character in upper case */
            int ch = *name++;
            if (ch >= 'a' && ch <= 'z') {
                ch = ch - 'a' + 'A';
            }
            if (*name2 != ch) {
                return 0;
            }
            --len;
        }
        ++name2;
        ++posn;
    }
    if (len == 0 && *name2 == '\0') {
        /* We have matched a whole keyword, so it is not a prefix */
        *prefix = 0;
        return 1;
    } else {
        if (posn != 0) {
            /* We have already matched a keyword prefix, which means
             * that the preceding words cannot be a variable name */
            *prefix = 1;
        }
        return 0;
    }
}

const ip_token_info_t *ip_tokeniser_lookup_keyword
    (const char *name, size_t len, unsigned context, int *prefix)
{
    const ip_token_info_t *info;
    size_t tlen;
    if (!len) {
        return 0;
    }
    *prefix = 0;
    for (info = tokens; info->name != 0; ++info) {
        /* Ignore the token if it is an extension but we are not
         * currently parsing the extended syntax. */
        if ((info->flags & ITOK_TYPE_EXTENSION) != 0) {
            if ((context & ITOK_TYPE_EXTENSION) == 0) {
                continue;
            }
        }

        /* Handle punctuation tokens, which must match exactly */
        tlen = strlen(info->name);
        if (!ip_tokeniser_is_alpha(name[0]) ||
                !ip_tokeniser_is_alpha(info->name[0])) {
            if (len == tlen && !memcmp(name, info->name, len)) {
                return info;
            }
            continue;
        }

        /* Match against an alphabetic keyword */
        if (ip_tokeniser_match_keyword(name, len, info->name, prefix)) {
            return info;
        }
    }
    return 0;
}
