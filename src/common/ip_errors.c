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

#include "ip_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void ip_error(ip_parser_t *parser, const char *format, ...)
{
    va_list va;
    if (parser->tokeniser.loc.filename) {
        fprintf(stderr, "%s:%lu: ", parser->tokeniser.loc.filename,
                parser->tokeniser.loc.line);
    } else {
        fprintf(stderr, "%lu: ", parser->tokeniser.loc.line);
    }
    if (format) {
        va_start(va, format);
        vfprintf(stderr, format, va);
        va_end(va);
    } else {
        fputs("syntax error", stderr);
    }
    putc('\n', stderr);
    ++(parser->num_errors);
}

void ip_error_at
    (ip_parser_t *parser, const ip_loc_t *loc, const char *format, ...)
{
    va_list va;
    if (loc) {
        if (loc->filename) {
            fprintf(stderr, "%s:%lu: ", loc->filename, loc->line);
        } else {
            fprintf(stderr, "%lu: ", loc->line);
        }
    }
    if (format) {
        va_start(va, format);
        vfprintf(stderr, format, va);
        va_end(va);
    } else {
        fputs("syntax error", stderr);
    }
    putc('\n', stderr);
    ++(parser->num_errors);
}

void ip_error_near(ip_parser_t *parser, const char *format, ...)
{
    const char *name;
    va_list va;
    if (parser->tokeniser.loc.filename) {
        fprintf(stderr, "%s:%lu: ", parser->tokeniser.loc.filename,
                parser->tokeniser.loc.line);
    } else {
        fprintf(stderr, "%lu: ", parser->tokeniser.loc.line);
    }
    if (format) {
        va_start(va, format);
        vfprintf(stderr, format, va);
        putc('\n', stderr);
        va_end(va);
    } else {
        fputs("syntax error", stderr);
    }
    fprintf(stderr, ", at or near ");
    switch (parser->tokeniser.token) {
    case ITOK_EOL:
        fputs("<EOL>", stderr);
        break;

    case ITOK_EOF:
        fputs("<EOF>", stderr);
        break;

    default:
        name = parser->tokeniser.token_info->name;
        putc('"', stderr);
        while (name && *name != '\0') {
            int ch = *name++;
            if (ch < 0x20 || ch > 0x7E) {
                fprintf(stderr, "\\x%02X", ch & 0xFF);
            } else {
                putc(ch, stderr);
            }
        }
        putc('"', stderr);
        break;
    }
    putc('\n', stderr);
    ++(parser->num_errors);
}

void ip_warning(ip_parser_t *parser, const char *format, ...)
{
    va_list va;
    if (parser->tokeniser.loc.filename) {
        fprintf(stderr, "%s:%lu: ", parser->tokeniser.loc.filename,
                parser->tokeniser.loc.line);
    } else {
        fprintf(stderr, "%lu: ", parser->tokeniser.loc.line);
    }
    fputs("warning: ", stderr);
    if (format) {
        va_start(va, format);
        vfprintf(stderr, format, va);
        va_end(va);
    } else {
        fputs("syntax warning", stderr);
    }
    putc('\n', stderr);
    ++(parser->num_warnings);
}

void ip_warning_at
    (ip_parser_t *parser, const ip_loc_t *loc, const char *format, ...)
{
    va_list va;
    if (loc) {
        if (loc->filename) {
            fprintf(stderr, "%s:%lu: ", loc->filename, loc->line);
        } else {
            fprintf(stderr, "%lu: ", loc->line);
        }
    }
    fputs("warning: ", stderr);
    if (format) {
        va_start(va, format);
        vfprintf(stderr, format, va);
        va_end(va);
    } else {
        fputs("syntax warning", stderr);
    }
    putc('\n', stderr);
    ++(parser->num_warnings);
}

void ip_out_of_memory(void)
{
    fputs("out of memory\n", stderr);
    exit(2);
}
