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

#include "ip_program.h"
#include <stdlib.h>
#include <string.h>

ip_program_t *ip_program_new(const char *filename)
{
    ip_program_t *program = calloc(1, sizeof(ip_program_t));
    if (!program) {
        ip_out_of_memory();
    }
    ip_var_table_init(&(program->vars));
    ip_label_table_init(&(program->labels));
    ip_ast_list_init(&(program->statements));
    program->filename = strdup(filename);
    if (!(program->filename)) {
        ip_program_free(program);
        ip_out_of_memory();
    }
    return program;
}

void ip_program_free(ip_program_t *program)
{
    ip_var_table_free(&(program->vars));
    ip_label_table_free(&(program->labels));
    ip_ast_list_free(&(program->statements));
    free(program->filename);
    free(program);
}
