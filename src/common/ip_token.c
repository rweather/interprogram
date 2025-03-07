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
    {"IS",                                  ITOK_IS,                ITOK_TYPE_CONDITION},
    {"GREATER THAN",                        ITOK_GREATER_THAN,      ITOK_TYPE_CONDITION},
    {"MUCH GREATER THAN",                   ITOK_MUCH_GREATER_THAN, ITOK_TYPE_CONDITION},
    {"SMALLER THAN",                        ITOK_SMALLER_THAN,      ITOK_TYPE_CONDITION},
    {"MUCH SMALLER THAN",                   ITOK_MUCH_SMALLER_THAN, ITOK_TYPE_CONDITION},
    {"ZERO",                                ITOK_ZERO,              ITOK_TYPE_CONDITION},
    {"POSITIVE",                            ITOK_POSITIVE,          ITOK_TYPE_CONDITION},
    {"NEGATIVE",                            ITOK_NEGATIVE,          ITOK_TYPE_CONDITION},
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
    {"IS NOT",                              ITOK_IS_NOT,            ITOK_TYPE_CONDITION | ITOK_TYPE_EXTENSION},
    {"EQUAL TO",                            ITOK_EQUAL_TO,          ITOK_TYPE_CONDITION | ITOK_TYPE_EXTENSION},
    {"GREATER THAN OR EQUAL TO",            ITOK_GREATER_OR_EQUAL,  ITOK_TYPE_CONDITION | ITOK_TYPE_EXTENSION},
    {"SMALLER THAN OR EQUAL TO",            ITOK_SMALLER_OR_EQUAL,  ITOK_TYPE_CONDITION | ITOK_TYPE_EXTENSION},
    {"FINITE",                              ITOK_FINITE,            ITOK_TYPE_CONDITION | ITOK_TYPE_EXTENSION},
    {"INFINITE",                            ITOK_INFINITE,          ITOK_TYPE_CONDITION | ITOK_TYPE_EXTENSION},
    {"A NUMBER",                            ITOK_A_NUMBER,          ITOK_TYPE_CONDITION | ITOK_TYPE_EXTENSION},
    {"MODULO",                              ITOK_MODULO,            ITOK_TYPE_STATEMENT | ITOK_TYPE_EXPRESSION | ITOK_TYPE_EXTENSION},
    {"*",                                   ITOK_MUL,               ITOK_TYPE_EXPRESSION | ITOK_TYPE_EXTENSION},
    {"/",                                   ITOK_DIV,               ITOK_TYPE_EXPRESSION | ITOK_TYPE_EXTENSION},
    {"CALL",                                ITOK_CALL,              ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"RETURN",                              ITOK_RETURN,            ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {":",                                   ITOK_COLON,             ITOK_TYPE_ANY | ITOK_TYPE_EXTENSION},
    {"EMPTY",                               ITOK_EMPTY,             ITOK_TYPE_CONDITION | ITOK_TYPE_EXTENSION},
    {"LENGTH OF",                           ITOK_LENGTH_OF,         ITOK_TYPE_EXPRESSION | ITOK_TYPE_EXTENSION},
    {"SUBSTRING FROM",                      ITOK_SUBSTRING,         ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"TO",                                  ITOK_TO,                ITOK_TYPE_STATEMENT | ITOK_TYPE_EXPRESSION | ITOK_TYPE_EXTENSION},
    {"SYMBOLS FOR STRINGS",                 ITOK_SYMBOLS_STR,       ITOK_TYPE_PRELIM_2 | ITOK_TYPE_EXTENSION},
    {"EXIT INTERPROGRAM",                   ITOK_EXIT_PROGRAM,      ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"THEN",                                ITOK_THEN,              ITOK_TYPE_EXPRESSION | ITOK_TYPE_EXTENSION},
    {"ELSE",                                ITOK_ELSE,              ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"ELSE IF",                             ITOK_ELSE_IF,           ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"END IF",                              ITOK_END_IF,            ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"REPEAT WHILE",                        ITOK_REPEAT_WHILE,      ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"END REPEAT",                          ITOK_END_REPEAT,        ITOK_TYPE_STATEMENT | ITOK_TYPE_EXTENSION},
    {"SYMBOLS FOR ROUTINES",                ITOK_SYMBOLS_ROUTINES,  ITOK_TYPE_PRELIM_2 | ITOK_TYPE_EXTENSION},
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
            ip_out_of_memory();
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
            ip_out_of_memory();
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
    tokeniser->unget_char = -1;
    tokeniser->integer_precision = sizeof(ip_uint_t) * 8;
    ip_tokeniser_set_token(tokeniser, ITOK_ERROR);
    ip_symbol_table_init(&(tokeniser->routines));
}

void ip_tokeniser_free(ip_tokeniser_t *tokeniser)
{
    ip_symbol_table_free(&(tokeniser->routines));
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
    uint64_t ivalue;
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
    limit = ((ip_uint_t)1) << (tokeniser->integer_precision - 1);
    if ((context & ITOK_TYPE_NEGATIVE) == 0) {
        --limit;
    }

    /* We are done if we already know if the value is floating-point.
     * It is possible that integer values may be out of range for the
     * integer precision.  In that case we switch to floating-point. */
    if (!is_float && tokeniser->name_len <= 19) {
        ivalue = 0;
        for (posn = 0; posn < tokeniser->name_len; ++posn) {
            if (ivalue >= 10000000000000000000ULL) {
                break;
            }
            ivalue *= 10;
            ivalue += (unsigned)(tokeniser->name[posn] - '0');
        }
        tokeniser->ivalue = (ip_uint_t)ivalue;
        if (posn >= tokeniser->name_len && ivalue <= limit) {
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
    const char *found_routine = 0;
    const ip_token_info_t *found_info = 0;
    const ip_token_info_t *info;
    size_t posn = tokeniser->buffer_posn - 1;
    size_t end_var = posn;
    size_t end_keyword = posn;
    size_t found_len = 0;
    int ch;
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
            (tokeniser->buffer + posn, tokeniser->buffer_posn - posn, context);
        if (info) {
            /* Full keyword found.  Keep looking because we may find a
             * longer keyword later in the token table. */
            if (strlen(info->name) > found_len) {
                found_info = info;
                found_len = strlen(info->name);
                found_routine = 0;
                end_keyword = tokeniser->buffer_posn;
            }
        } else {
            /* Try looking for a registered routine with the name */
            const char *routine = ip_tokeniser_is_routine_name
                (tokeniser, tokeniser->buffer + posn,
                 tokeniser->buffer_posn - posn);
            if (routine && strlen(routine) > found_len) {
                found_info = 0;
                found_len = strlen(routine);
                found_routine = routine;
                end_keyword = tokeniser->buffer_posn;
            }
        }

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

    /* Did we find a suitable keyword? */
    if (found_info) {
        /* Built-in keyword */
        tokeniser->buffer_posn = end_keyword;
        ip_tokeniser_set_token_info(tokeniser, found_info);
        return;
    } else if (found_routine) {
        /* User-defined multi-word routine name */
        tokeniser->buffer_posn = end_keyword;
        tokeniser->name_len = 0;
        ip_tokeniser_add_name_chars
            (tokeniser, found_routine, strlen(found_routine));
        ip_tokeniser_set_token(tokeniser, ITOK_ROUTINE_NAME);
        return;
    }

    /* We have a variable name */
    tokeniser->buffer_posn = end_var;
    tokeniser->name_len = 0;
    ip_tokeniser_add_name_chars
        (tokeniser, tokeniser->buffer + posn, tokeniser->buffer_posn - posn);
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

/**
 * @brief Gets the next line of input.
 *
 * @param[in,out] tokeniser The tokeniser to read from.
 *
 * @return Non-zero if the line was read, zero at EOF.
 */
static int ip_tokeniser_get_line(ip_tokeniser_t *tokeniser)
{
    int ch;
    if (tokeniser->saw_eof) {
        /* We already saw EOF last time, so we are done */
        ip_tokeniser_set_token(tokeniser, ITOK_EOF);
        return 0;
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
                return 0;
            }

            /* Last line before EOF is missing EOL, so add it */
            ip_tokeniser_add_line(tokeniser, '\n');
        } else if (ch != '\0') {
            ip_tokeniser_add_line(tokeniser, ch);
        }
    }
    return 1;
}

/**
 * @brief Gets a string constant from the input.
 *
 * @param[in,out] tokeniser The tokeniser to read from.
 * @param[in] ch The quote character that started the string.
 */
static void ip_tokeniser_get_string(ip_tokeniser_t *tokeniser, int quote)
{
    int ch;

    /* Drop the leading quote from the in-progress string */
    tokeniser->name_len = 0;

    /* Get the characters for the rest of the string */
    while (tokeniser->buffer_posn < tokeniser->buffer_len) {
        ch = tokeniser->buffer[tokeniser->buffer_posn];
        if (ch == '\n') {
            /* String ends implicitly at the end of the line */
            break;
        } else if (ch == quote) {
            /* If the quote is doubled, then it indicates an escaped quote.
             * Otherwise it is the end of the string. */
            ++(tokeniser->buffer_posn);
            if (tokeniser->buffer_posn >= tokeniser->buffer_len ||
                    tokeniser->buffer[tokeniser->buffer_posn] != quote) {
                break;
            }
        } else if (ch == '\\') {
            /* Recognise a limited number of C-like escape sequences. */
            ++(tokeniser->buffer_posn);
            if ((tokeniser->buffer_posn >= tokeniser->buffer_len) ||
                    tokeniser->buffer[tokeniser->buffer_posn] == '\n') {
                /* Backslash at the end of the line continues the
                 * string on the next line. */
                if (!ip_tokeniser_get_line(tokeniser)) {
                    break;
                }
                continue;
            }
            ch = tokeniser->buffer[tokeniser->buffer_posn];
            switch (ch) {
            case 'a':   ip_tokeniser_add_name(tokeniser, '\a'); break;
            case 'b':   ip_tokeniser_add_name(tokeniser, '\b'); break;
            case 'e':   ip_tokeniser_add_name(tokeniser, 0x1B); break;
            case 'f':   ip_tokeniser_add_name(tokeniser, '\f'); break;
            case 'n':   ip_tokeniser_add_name(tokeniser, '\n'); break;
            case 'r':   ip_tokeniser_add_name(tokeniser, '\r'); break;
            case 't':   ip_tokeniser_add_name(tokeniser, '\t'); break;
            case 'v':   ip_tokeniser_add_name(tokeniser, '\v'); break;
            default:    ip_tokeniser_add_name(tokeniser, ch); break;
            }
        }
        ip_tokeniser_add_name(tokeniser, ch);
        ++(tokeniser->buffer_posn);
    }
    ip_tokeniser_set_token(tokeniser, ITOK_STR_VALUE);
}

/**
 * @brief Reads the rest of the input as a single literal string.
 *
 * @param[in,out] tokeniser The tokeniser.
 */
static void ip_tokeniser_read_rest(ip_tokeniser_t *tokeniser)
{
    int ch;
    while (!(tokeniser->saw_eof)) {
        if (tokeniser->unget_char == -1) {
            ch = (*(tokeniser->read_char))(tokeniser->user_data);
        } else {
            ch = tokeniser->unget_char;
            tokeniser->unget_char = -1;
        }
        if (ch == -1) {
            tokeniser->saw_eof = 1;
        } else if (ch == '\r') {
            /* Check for CRLF and convert it into just LF */
            ch = (*(tokeniser->read_char))(tokeniser->user_data);
            if (ch == -1) {
                tokeniser->saw_eof = 1;
            } else if (ch != '\n') {
                tokeniser->unget_char = ch;
            }
            ip_tokeniser_add_name(tokeniser, '\n');
        } else if (ch != '\0') {
            ip_tokeniser_add_name(tokeniser, ch);
        }
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
        if (!ip_tokeniser_get_line(tokeniser)) {
            return ITOK_EOF;
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
    case '.':
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
    IP_SIMPLE_TOKEN('=', ITOK_EQUAL,        ITOK_TYPE_ANY)
    IP_SIMPLE_TOKEN('&', ITOK_AMPERSAND,    ITOK_TYPE_STATEMENT)
    IP_SIMPLE_TOKEN('+', ITOK_PLUS,         ITOK_TYPE_EXPRESSION)
    IP_SIMPLE_TOKEN('-', ITOK_MINUS,        ITOK_TYPE_EXPRESSION)
    IP_SIMPLE_TOKEN('/', ITOK_DIV,          ITOK_TYPE_EXPRESSION | ITOK_TYPE_EXTENSION)
    IP_SIMPLE_TOKEN(':', ITOK_COLON,        ITOK_TYPE_ANY | ITOK_TYPE_EXTENSION)

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

    case '\'': case '"':
        /* String constant in extended mode, error in classic mode */
        if ((context & ITOK_TYPE_EXTENSION) != 0) {
            ip_tokeniser_get_string(tokeniser, ch);
        } else {
            ip_tokeniser_set_token(tokeniser, ITOK_ERROR);
        }
        break;

    case '~':
        /* May be the end of the program code and the start of the
         * embedded input data. */
        if ((tokeniser->buffer_posn + 4) <= tokeniser->buffer_len &&
                tokeniser->buffer[tokeniser->buffer_posn]     == '~' &&
                tokeniser->buffer[tokeniser->buffer_posn + 1] == '~' &&
                tokeniser->buffer[tokeniser->buffer_posn + 2] == '~' &&
                tokeniser->buffer[tokeniser->buffer_posn + 3] == '~') {
            /* Discard the rest of the current line and read the
             * rest of the input as a literal string. */
            ++(tokeniser->line);
            tokeniser->buffer_posn = 0;
            tokeniser->buffer_len = 0;
            tokeniser->name_len = 0;
            ip_tokeniser_read_rest(tokeniser);
            ip_tokeniser_set_token(tokeniser, ITOK_INPUT_DATA);
        } else {
            /* Not enough blanks, so treat this as an error */
            ip_tokeniser_set_token(tokeniser, ITOK_ERROR);
        }
        break;

    case '@':
        /* Subroutine argument number in the extension syntax: "@n" */
        if (tokeniser->buffer_posn < tokeniser->buffer_len) {
            ch = tokeniser->buffer[tokeniser->buffer_posn];
        } else {
            ch = '\0';
        }
        if ((context & ITOK_TYPE_EXTENSION) != 0 && ch >= '1' && ch <= '9') {
            ip_tokeniser_add_name(tokeniser, ch);
            tokeniser->ivalue = ch - '1';
            ++(tokeniser->buffer_posn);
            ip_tokeniser_set_token(tokeniser, ITOK_ARG_NUMBER);
        } else {
            ip_tokeniser_set_token(tokeniser, ITOK_ERROR);
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
 *
 * @return Non-zero if the keyword matches, zero if not.
 */
static int ip_tokeniser_match_keyword
    (const char *name, size_t len, const char *name2)
{
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
    }
    return (len == 0 && *name2 == '\0');
}

const ip_token_info_t *ip_tokeniser_lookup_keyword
    (const char *name, size_t len, unsigned context)
{
    const ip_token_info_t *info;
    size_t tlen;
    if (!len) {
        return 0;
    }
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
        if (ip_tokeniser_match_keyword(name, len, info->name)) {
            return info;
        }
    }
    return 0;
}

char *ip_tokeniser_read_punch(ip_tokeniser_t *tokeniser)
{
    size_t posn;
    int ch;

    /* Set the token details for the punch text */
    tokeniser->token = ITOK_PUNCH;
    tokeniser->name_len = 0;
    tokeniser->ivalue = 0;
    tokeniser->fvalue = 0;
    tokeniser->token_info = &(tokeniser->token_space);
    tokeniser->token_space.name = 0;
    tokeniser->token_space.code = ITOK_PUNCH;
    tokeniser->token_space.flags = 0;
    tokeniser->loc.filename = tokeniser->filename;
    tokeniser->loc.line = tokeniser->line;

    /* Discard the rest of the current line and read the following lines */
    while (ip_tokeniser_get_line(tokeniser)) {
        /* Search for "~~~~~" to indicate the end of the punch text.
         * Any other characters are added to the punch text. */
        posn = 0;
        while (posn < tokeniser->buffer_len) {
            ch = tokeniser->buffer[posn];
            if (ch == '~' && (tokeniser->buffer_len - posn) >= 5) {
                /* We have a single '~'.  Do we have four more? */
                if (tokeniser->buffer[posn + 1] == '~' &&
                        tokeniser->buffer[posn + 2] == '~' &&
                        tokeniser->buffer[posn + 3] == '~' &&
                        tokeniser->buffer[posn + 4] == '~') {
                    /* We have found the end of the punch text.  Skip any
                     * additional '~' characters after the first five. */
                    posn += 5;
                    while (posn < tokeniser->buffer_len) {
                        ch = tokeniser->buffer[posn];
                        if (ch != '~') {
                            break;
                        }
                        ++posn;
                    }

                    /* Return the finished punch text to the caller */
                    ip_tokeniser_add_name(tokeniser, '\0');
                    tokeniser->token_space.name = tokeniser->name;
                    tokeniser->buffer_posn = posn;
                    return tokeniser->name;
                } else {
                    ip_tokeniser_add_name(tokeniser, ch);
                }
            } else if (ch != '\0') {
                ip_tokeniser_add_name(tokeniser, ch);
            }
            ++posn;
        }
    }

    /* If we get here then we have reached EOF */
    ip_tokeniser_add_name(tokeniser, '\0');
    tokeniser->token_space.name = tokeniser->name;
    return tokeniser->name;
}

char *ip_tokeniser_read_title(ip_tokeniser_t *tokeniser)
{
    size_t end;
    int ch;

    /* Set the token details for the title text */
    tokeniser->token = ITOK_TITLE;
    tokeniser->name_len = 0;
    tokeniser->ivalue = 0;
    tokeniser->fvalue = 0;
    tokeniser->token_info = &(tokeniser->token_space);
    tokeniser->token_space.name = 0;
    tokeniser->token_space.code = ITOK_TITLE;
    tokeniser->token_space.flags = 0;
    tokeniser->loc.filename = tokeniser->filename;
    tokeniser->loc.line = tokeniser->line;

    /* Skip whitespace after "TITLE" but before the actual title */
    while (tokeniser->buffer_posn < tokeniser->buffer_len) {
        ch = tokeniser->buffer[tokeniser->buffer_posn];
        if (ip_tokeniser_is_space(ch) || ch == '\n') {
            ++(tokeniser->buffer_posn);
        } else {
            break;
        }
    }

    /* Strip whitespace from the end of the line */
    end = tokeniser->buffer_len;
    while (end > tokeniser->buffer_posn) {
        ch = tokeniser->buffer[end - 1];
        if (!ip_tokeniser_is_space(ch) && ch != '\n') {
            break;
        }
        --end;
    }

    /* Return the title to the caller */
    ip_tokeniser_add_name_chars
        (tokeniser, tokeniser->buffer + tokeniser->buffer_posn,
         end - tokeniser->buffer_posn);
    tokeniser->buffer_posn = tokeniser->buffer_len; /* Done with this line */
    ip_tokeniser_add_name(tokeniser, '\0');
    tokeniser->token_space.name = tokeniser->name;
    return tokeniser->name;
}

void ip_tokeniser_skip_line(ip_tokeniser_t *tokeniser)
{
    tokeniser->buffer_posn = tokeniser->buffer_len;
}

int ip_tokeniser_lookahead(ip_tokeniser_t *tokeniser, int ch)
{
    size_t posn = tokeniser->buffer_posn;
    int lookahead;
    while (posn < tokeniser->buffer_len) {
        lookahead = tokeniser->buffer[posn++];
        if (lookahead == ch) {
            return 1;
        }
        if (!ip_tokeniser_is_space(lookahead)) {
            break;
        }
    }
    return 0;
}

void ip_tokeniser_register_routine_name
    (ip_tokeniser_t *tokeniser, const char *name)
{
    ip_symbol_t *routine;

    /* Single-word routine names do not need to be registered */
    if (strchr(name, ' ') == 0) {
        return;
    }

    /* Register the routine */
    routine = calloc(1, sizeof(ip_symbol_t));
    if (!routine) {
        ip_out_of_memory();
    }
    routine->name = strdup(name);
    if (!(routine->name)) {
        ip_out_of_memory();
    }
    ip_symbol_insert(&(tokeniser->routines), routine);
}

static const char *ip_tokeniser_find_routine_name
    (const ip_tokeniser_t *tokeniser, const char *name, size_t len,
     ip_symbol_t *routine)
{
    const char *result;
    if (routine && routine != &(tokeniser->routines.nil)) {
        if (ip_tokeniser_match_keyword(name, len, routine->name)) {
            return routine->name;
        }
        result = ip_tokeniser_find_routine_name
            (tokeniser, name, len, routine->left);
        if (result) {
            return result;
        }
        return ip_tokeniser_find_routine_name
            (tokeniser, name, len, routine->right);
    }
    return 0;
}

const char *ip_tokeniser_is_routine_name
    (const ip_tokeniser_t *tokeniser, const char *name, size_t len)
{
    return ip_tokeniser_find_routine_name
        (tokeniser, name, len, tokeniser->routines.root.right);
}
