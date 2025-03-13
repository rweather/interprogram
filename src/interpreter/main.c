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
#include "ip_exec.h"
#include "ip_math_lib.h"
#include "ip_string_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define short_options "o:i:cev"
static struct option long_options[] = {
    {"output",      required_argument,  0,  'o'},
    {"input",       required_argument,  0,  'i'},
    {"classic",     no_argument,        0,  'c'},
    {"extended",    no_argument,        0,  'e'},
    {"verify-chars",no_argument,        0,  'v'},
    {0,             0,                  0,  0},
};

static void usage(const char *progname)
{
    fprintf(stderr, "Usage: %s program [input]\n\n", progname);

    fprintf(stderr, "--output FILE, -o FILE\n");
    fprintf(stderr, "    Set the output file (default is standard output).\n\n");

    fprintf(stderr, "--input FILE, -i FILE\n");
    fprintf(stderr, "    Set the input file (default is standard input).\n\n");

    fprintf(stderr, "--classic, -c\n");
    fprintf(stderr, "    Force the use of the classic INTERPROGRAM syntax.\n\n");

    fprintf(stderr, "--extended, -e\n");
    fprintf(stderr, "    Force the use of the extended INTERPROGRAM syntax.\n\n");

    fprintf(stderr, "--verify-chars, -v\n");
    fprintf(stderr, "    Verify that only Flexowriter-compatible characters are in use.\n\n");
}

static void register_builtins(ip_parser_t *parser, unsigned options)
{
    ip_register_math_builtins(parser->program, options);
    ip_register_string_builtins(parser->program, options);
}

/* Verify that a file only contains characters compatible with the
 * original Flexowriter teleprinter on the CSIRAC. */
static int verify_characters(const char *filename)
{
    /* Note: ~ stands in for the original Flexowriter "blank", and
     * $ stands in for the original Flexowriter pound sign. */
    static char const allowed_chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 \t\r\n+-*/()#=&,.'x$~";
    FILE *file;
    int exitval = 0;
    int ch;
    unsigned long line = 1;
    if ((file = fopen(filename, "r")) == NULL) {
        perror(filename);
        return 1;
    }
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\0' || strchr(allowed_chars, ch) == 0) {
            fprintf(stderr, "%s:%lu: invalid character '%c'\n",
                    filename, line, ch);
            exitval = 1;
        }
        if (ch == '\n') {
            ++line;
        }
    }
    fclose(file);
    return exitval;
}

int main(int argc, char **argv)
{
    const char *progname = argv[0];
    unsigned options = 0;
    int verify_chars = 0;
    const char *program_filename = 0;
    const char *input_filename = 0;
    const char *output_filename = 0;
    ip_program_t *program = 0;
    ip_exec_t exec;
    int opt, index;
    int exitval = 0;
    FILE *input = stdin;
    FILE *output = stdout;

    /* Parse the command-line options */
    while ((opt = getopt_long
                    (argc, argv, short_options, long_options, &index)) >= 0) {
        switch (opt) {
        case 'o':
            output_filename = optarg;
            break;

        case 'i':
            input_filename = optarg;
            break;

        case 'c':
            options &= ~ITOK_TYPE_EXTENSION;
            options |= ITOK_TYPE_CLASSIC;
            break;

        case 'e':
            options &= ~ITOK_TYPE_CLASSIC;
            options |= ITOK_TYPE_EXTENSION;
            break;

        case 'v':
            verify_chars = 1;
            break;

        default:
            usage(progname);
            return 1;
        }
    }

    /* Need at least one option for the program source file */
    if (optind >= argc) {
        usage(progname);
        return 1;
    }
    program_filename = argv[optind];

    /* Are we verifying the characters? */
    if (verify_chars) {
        return verify_characters(program_filename);
    }

    /* Create the program object and register built-in statements */
    program = ip_program_new(program_filename);

    /* Load the program into memory */
    if (ip_parse_program_file
            (program, program_filename, options,
             argc - optind, argv + optind, register_builtins) != 0) {
        ip_program_free(program);
        return 1;
    }

    /* Open the input and output sources */
    if (input_filename) {
        input = fopen(input_filename, "r");
        if (!input) {
            perror(input_filename);
            ip_program_free(program);
            return 1;
        }
    }
    if (output_filename) {
        output = fopen(output_filename, "w");
        if (!output) {
            perror(output_filename);
            if (input_filename) {
                fclose(input);
            }
            ip_program_free(program);
            return 1;
        }
    }

    /* Run the program */
    ip_exec_init(&exec, program);
    exec.input = input;
    exec.output = output;
    exitval = ip_exec_run(&exec);
    ip_exec_free(&exec);

    /* Clean up and exit */
    if (input_filename) {
        fclose(input);
    }
    if (output_filename) {
        fclose(output);
    }
    return exitval;
}
