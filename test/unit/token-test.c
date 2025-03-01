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

static int check_keyword(const char *name, int id)
{
    const ip_token_info_t *info;
    int exitval = 0;
    info = ip_tokeniser_get_keyword(id);
    if (!info || strcmp(name, info->name) != 0 || id != info->code) {
        printf("0x%02X, \"%s\" is not correct\n", id, name);
        exitval = 1;
    }
    if (id == ITOK_MUL) {
        /* Cannot look this up as a keyword because of conflicts with
         * ITOK_LABEL, so we are done */
        return exitval;
    } else {
        info = ip_tokeniser_lookup_keyword
            (name, strlen(name), ITOK_TYPE_ANY | ITOK_TYPE_EXTENSION);
    }
    if (!info) {
        printf("0x%02X, \"%s\" could not be looked up\n", id, name);
        exitval = 1;
    } else {
        if (strcmp(name, info->name) != 0 || id != info->code) {
            printf("0x%02X, \"%s\" lookup is not correct\n", id, name);
            exitval = 1;
        }
    }
    return exitval;
}

static int check_identifiers(void)
{
    unsigned char tested[ITOK_LAST_KEYWORD - ITOK_FIRST_KEYWORD + 1] = {0};
    int exitval = 0;
    int index;

    /* Check invalid identifiers */
    if (ip_tokeniser_get_keyword(0) != 0)
        return 1;
    if (ip_tokeniser_get_keyword(ITOK_FIRST_KEYWORD - 1) != 0)
        return 1;
    if (ip_tokeniser_get_keyword(ITOK_LAST_KEYWORD + 1) != 0)
        return 1;

    /* Check the mapping between keyword identifiers and names */
#define KEYWORD(n, id) \
    do { \
        exitval |= check_keyword((n), (id)); \
        tested[(id) - ITOK_FIRST_KEYWORD] = 1; \
    } while (0)
    KEYWORD(",",                                    ITOK_COMMA);
    KEYWORD("(",                                    ITOK_LPAREN);
    KEYWORD(")",                                    ITOK_RPAREN);
    KEYWORD("=",                                    ITOK_EQUAL);
    KEYWORD("*",                                    ITOK_LABEL);
    KEYWORD("&",                                    ITOK_AMPERSAND);
    KEYWORD("+",                                    ITOK_PLUS);
    KEYWORD("-",                                    ITOK_MINUS);
    KEYWORD("(1)",                                  ITOK_PRELIM_1);
    KEYWORD("(2)",                                  ITOK_PRELIM_2);
    KEYWORD("(3)",                                  ITOK_PRELIM_3);
    KEYWORD("(4)",                                  ITOK_PRELIM_4);
    KEYWORD("TITLE",                                ITOK_TITLE);
    KEYWORD("SYMBOLS FOR INTEGERS",                 ITOK_SYMBOLS_INT);
    KEYWORD("MAXIMUM SUBSCRIPTS",                   ITOK_MAX_SUBSCRIPTS);
    KEYWORD("COMPILE THE FOLLOWING INTERPROGRAM",   ITOK_COMPILE_PROGRAM);
    KEYWORD("NONE",                                 ITOK_NONE);
    KEYWORD("END OF INTERPROGRAM",                  ITOK_END_PROGRAM);
    KEYWORD("END OF PROCESS DEFINITION",            ITOK_END_PROCESS);
    KEYWORD("THIS",                                 ITOK_THIS);
    KEYWORD("TAKE",                                 ITOK_TAKE);
    KEYWORD("ADD",                                  ITOK_ADD);
    KEYWORD("SUBTRACT",                             ITOK_SUBTRACT);
    KEYWORD("MULTIPLY BY",                          ITOK_MULTIPLY);
    KEYWORD("DIVIDE BY",                            ITOK_DIVIDE);
    KEYWORD("IF",                                   ITOK_IF);
    KEYWORD("IS",                                   ITOK_IS);
    KEYWORD("GREATER THAN",                         ITOK_GREATER_THAN);
    KEYWORD("MUCH GREATER THAN",                    ITOK_MUCH_GREATER_THAN);
    KEYWORD("SMALLER THAN",                         ITOK_SMALLER_THAN);
    KEYWORD("MUCH SMALLER THAN",                    ITOK_MUCH_SMALLER_THAN);
    KEYWORD("ZERO",                                 ITOK_ZERO);
    KEYWORD("POSITIVE",                             ITOK_POSITIVE);
    KEYWORD("NEGATIVE",                             ITOK_NEGATIVE);
    KEYWORD("REPLACE",                              ITOK_REPLACE);
    KEYWORD("INPUT",                                ITOK_INPUT);
    KEYWORD("OUTPUT",                               ITOK_OUTPUT);
    KEYWORD("SET",                                  ITOK_SET);
    KEYWORD("GO TO",                                ITOK_GO_TO);
    KEYWORD("EXECUTE PROCESS",                      ITOK_EXECUTE_PROCESS);
    KEYWORD("REPEAT FROM",                          ITOK_REPEAT_FROM);
    KEYWORD("TIMES",                                ITOK_TIMES);
    KEYWORD("PAUSE",                                ITOK_PAUSE);
    KEYWORD("PUNCH THE FOLLOWING CHARACTERS",       ITOK_PUNCH);
    KEYWORD("COPY TAPE",                            ITOK_COPY_TAPE);
    KEYWORD("IGNORE TAPE",                          ITOK_IGNORE_TAPE);
    KEYWORD("FORM SQUARE ROOT",                     ITOK_SQRT);
    KEYWORD("FORM SINE",                            ITOK_SIN);
    KEYWORD("FORM COSINE",                          ITOK_COS);
    KEYWORD("FORM TANGENT",                         ITOK_TAN);
    KEYWORD("FORM ARCTAN",                          ITOK_ATAN);
    KEYWORD("FORM NATURAL LOG",                     ITOK_LOG);
    KEYWORD("FORM EXPONENTIAL",                     ITOK_EXP);
    KEYWORD("FORM ABSOLUTE",                        ITOK_ABS);
    KEYWORD("FORM SINE RADIANS",                    ITOK_SIN_RADIANS);
    KEYWORD("FORM COSINE RADIANS",                  ITOK_COS_RADIANS);
    KEYWORD("FORM TANGENT RADIANS",                 ITOK_TAN_RADIANS);
    KEYWORD("FORM ARCTAN RADIANS",                  ITOK_ATAN_RADIANS);
    KEYWORD("FORM SINE DEGREES",                    ITOK_SIN_DEGREES);
    KEYWORD("FORM COSINE DEGREES",                  ITOK_COS_DEGREES);
    KEYWORD("FORM TANGENT DEGREES",                 ITOK_TAN_DEGREES);
    KEYWORD("FORM ARCTAN DEGREES",                  ITOK_ATAN_DEGREES);
    KEYWORD("IS NOT",                               ITOK_IS_NOT);
    KEYWORD("EQUAL TO",                             ITOK_EQUAL_TO);
    KEYWORD("GREATER THAN OR EQUAL TO",             ITOK_GREATER_OR_EQUAL);
    KEYWORD("SMALLER THAN OR EQUAL TO",             ITOK_SMALLER_OR_EQUAL);
    KEYWORD("FINITE",                               ITOK_FINITE);
    KEYWORD("INFINITE",                             ITOK_INFINITE);
    KEYWORD("A NUMBER",                             ITOK_A_NUMBER);
    KEYWORD("MODULO",                               ITOK_MODULO);
    KEYWORD("BITWISE AND WITH NOT",                 ITOK_BITWISE_AND_NOT);
    KEYWORD("BITWISE AND WITH",                     ITOK_BITWISE_AND);
    KEYWORD("BITWISE OR WITH",                      ITOK_BITWISE_OR);
    KEYWORD("BITWISE XOR WITH",                     ITOK_BITWISE_XOR);
    KEYWORD("BITWISE NOT",                          ITOK_BITWISE_NOT);
    KEYWORD("SHIFT LEFT BY",                        ITOK_SHIFT_LEFT);
    KEYWORD("SHIFT RIGHT BY",                       ITOK_SHIFT_RIGHT);
    KEYWORD("*",                                    ITOK_MUL);
    KEYWORD("/",                                    ITOK_DIV);
    KEYWORD("RAISE TO THE POWER OF",                ITOK_RAISE);
    KEYWORD("CALL",                                 ITOK_CALL);
    KEYWORD("RETURN",                               ITOK_RETURN);
    KEYWORD(":",                                    ITOK_COLON);
    KEYWORD("EMPTY",                                ITOK_EMPTY);
    KEYWORD("LENGTH OF",                            ITOK_LENGTH_OF);
    KEYWORD("SUBSTRING FROM",                       ITOK_SUBSTRING);
    KEYWORD("TO",                                   ITOK_TO);
    KEYWORD("SYMBOLS FOR STRINGS",                  ITOK_SYMBOLS_STR);
    KEYWORD("EXIT INTERPROGRAM",                    ITOK_EXIT_PROGRAM);
    KEYWORD("RANDOM NUMBER",                        ITOK_RANDOM);
    KEYWORD("SEED RANDOM",                          ITOK_SEED_RANDOM);
    KEYWORD("THEN",                                 ITOK_THEN);
    KEYWORD("ELSE",                                 ITOK_ELSE);
    KEYWORD("ELSE IF",                              ITOK_ELSE_IF);
    KEYWORD("END IF",                               ITOK_END_IF);
    KEYWORD("REPEAT WHILE",                         ITOK_REPEAT_WHILE);
    KEYWORD("END REPEAT",                           ITOK_END_REPEAT);

    /* Check that we have tested all of the keywords */
    for (index = ITOK_FIRST_KEYWORD; index <= ITOK_LAST_KEYWORD; ++index) {
        if (!tested[index - ITOK_FIRST_KEYWORD]) {
            printf("0x%02X was not checked\n", index);
            exitval = 1;
        }
    }

    return exitval;
}

typedef struct
{
    const char *data;
    int eol;

} test_stream_t;

static void test_stream_init(test_stream_t *stream, const char *data)
{
    stream->data = data;
    stream->eol = 1;
}

static int test_stream_read(void *user_data)
{
    test_stream_t *stream = (test_stream_t *)user_data;
    if (stream->data && stream->data[0] != '\0') {
        int ch = stream->data[0] & 0xFF;
        ++(stream->data);
        return ch;
    } else if (stream->eol) {
        stream->eol = 0;
        return '\n';
    } else {
        return -1;
    }
}

static int check_lexer(const char *str, int token, unsigned context)
{
    test_stream_t stream;
    ip_tokeniser_t tokeniser;
    int ok = 0;

    test_stream_init(&stream, str);

    ip_tokeniser_init(&tokeniser);
    tokeniser.read_char = test_stream_read;
    tokeniser.user_data = &stream;
    tokeniser.filename = "dummy.ip";

    if (ip_tokeniser_get_next(&tokeniser, context) == token) {
        if (ip_tokeniser_get_next(&tokeniser, context) == ITOK_EOL) {
            if (ip_tokeniser_get_next(&tokeniser, context) == ITOK_EOF) {
                ok = 1;
            }
        }
    }

    ip_tokeniser_free(&tokeniser);
    return !ok;
}

int main(int argc, char **argv)
{
    int exitval = 0;

    (void)argc;
    (void)argv;

#define RUN_TEST(name) \
    do { \
        printf(#name " ... "); \
        fflush(stdout); \
        if (name()) { \
            printf("FAILED\n"); \
            exitval = 1; \
        } else { \
            printf("ok\n"); \
        } \
    } while (0)
#define RUN_LEXER_TEST(str, token, context) \
    do { \
        printf("check_lexer[%s] ... ", (str)); \
        fflush(stdout); \
        if (check_lexer((str), (token), (context))) { \
            printf("FAILED\n"); \
            exitval = 1; \
        } else { \
            printf("ok\n"); \
        } \
    } while (0)

    RUN_TEST(check_identifiers);

    RUN_LEXER_TEST("var", ITOK_VAR_NAME, ITOK_TYPE_EXPRESSION);
    RUN_LEXER_TEST("FORM SQUARE ROOT", ITOK_SQRT, ITOK_TYPE_STATEMENT);
    RUN_LEXER_TEST("FORM", ITOK_VAR_NAME, ITOK_TYPE_STATEMENT | ITOK_TYPE_EXPRESSION);
    RUN_LEXER_TEST("FORMY", ITOK_VAR_NAME, ITOK_TYPE_STATEMENT | ITOK_TYPE_EXPRESSION);
    RUN_LEXER_TEST("THIS", ITOK_THIS, ITOK_TYPE_EXPRESSION);
    RUN_LEXER_TEST("NONE", ITOK_NONE, ITOK_TYPE_EXPRESSION);
    RUN_LEXER_TEST("NONE", ITOK_NONE, ITOK_TYPE_SYMBOL_NAME);
    RUN_LEXER_TEST("MuLtIpLy By", ITOK_MULTIPLY, ITOK_TYPE_STATEMENT);
    RUN_LEXER_TEST("*", ITOK_LABEL, ITOK_TYPE_STATEMENT);
    RUN_LEXER_TEST("*", ITOK_LABEL, ITOK_TYPE_EXPRESSION);
    RUN_LEXER_TEST("*", ITOK_MUL, ITOK_TYPE_EXPRESSION | ITOK_TYPE_EXTENSION);

    return exitval;
}
