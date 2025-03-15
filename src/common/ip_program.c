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
    ip_symbol_table_init(&(program->builtins));
    program->filename = strdup(filename);
    if (!(program->filename)) {
        ip_program_free(program);
        ip_out_of_memory();
    }
    return program;
}

void ip_program_free(ip_program_t *program)
{
    if (program) {
        ip_var_table_free(&(program->vars));
        ip_label_table_free(&(program->labels));
        ip_ast_list_free(&(program->statements));
        ip_symbol_table_free(&(program->builtins));
        free(program->filename);
        if (program->embedded_input) {
            free(program->embedded_input);
        }
        free(program);
    }
}

void ip_program_reset_variables(ip_program_t *program)
{
    ip_var_table_reset(&(program->vars));
    program->next_input = program->embedded_input;
}

void ip_program_set_input(ip_program_t *program, const char *input)
{
    char *temp;
    if (input) {
        temp = strdup(input);
        if (!temp) {
            ip_out_of_memory();
        }
    } else {
        temp = 0;
    }
    if (program->embedded_input) {
        free(program->embedded_input);
    }
    program->embedded_input = temp;
    program->next_input = temp;
}

void ip_program_register_builtin
    (ip_program_t *program, const char *name,
     ip_builtin_handler_t handler, unsigned char min_args,
     unsigned char max_args)
{
    ip_builtin_t *builtin;
    ip_label_t *label;

    /* Do we already have this built-in? */
    builtin = (ip_builtin_t *)ip_symbol_lookup_by_name
        (&(program->builtins), name);
    if (builtin) {
        builtin->handler = handler;
        builtin->base.type = (unsigned char)(min_args | (max_args << 4));
        return;
    }

    /* Create a new built-in */
    builtin = calloc(1, sizeof(ip_builtin_t));
    if (!builtin) {
        ip_out_of_memory();
    }
    builtin->base.name = strdup(name);
    if (!(builtin->base.name)) {
        ip_out_of_memory();
    }
    builtin->base.num = -1;
    builtin->base.type = (unsigned char)(min_args | (max_args << 4));
    builtin->handler = handler;
    ip_symbol_insert(&(program->builtins), &(builtin->base));

    /* Create the corresponding routine label and variable so that the
     * parser will recognise this name for implicit "CALL"'s */
    if (min_args <= max_args) {
        label = ip_label_create_by_name(&(program->labels), name);
        label->base.type = IP_TYPE_ROUTINE;
        label->builtin = (void *)handler;
        ip_label_mark_as_defined(label);
        ip_var_create(&(program->vars), name, IP_TYPE_ROUTINE);
    }
}

void ip_program_register_builtins
    (ip_program_t *program, const ip_builtin_info_t *builtins)
{
    while (builtins && builtins->name) {
        ip_program_register_builtin
            (program, builtins->name, builtins->handler,
             builtins->min_args, builtins->max_args);
        ++builtins;
    }
}

ip_builtin_t *ip_program_lookup_builtin_routine
    (const ip_program_t *program, const char *name)
{
    ip_builtin_t *builtin =
        (ip_builtin_t *)ip_symbol_lookup_by_name(&(program->builtins), name);
    if (builtin) {
        int min_args = builtin->base.type & 0x0F;
        int max_args = (builtin->base.type >> 4) & 0x0F;
        if (min_args > max_args) {
            /* This is a built-in function, not a built-in routine */
            return 0;
        }
    }
    return builtin;
}

ip_builtin_t *ip_program_lookup_builtin_function
    (const ip_program_t *program, const char *name)
{
    ip_builtin_t *builtin =
        (ip_builtin_t *)ip_symbol_lookup_by_name(&(program->builtins), name);
    if (builtin) {
        int min_args = builtin->base.type & 0x0F;
        int max_args = (builtin->base.type >> 4) & 0x0F;
        if (min_args <= max_args) {
            /* This is a built-in routine, not a built-in function */
            return 0;
        }
    }
    return builtin;
}

int ip_builtin_validate_num_args(ip_builtin_t *builtin, int num_args)
{
    if (builtin) {
        int min_args = builtin->base.type & 0x0F;
        int max_args = (builtin->base.type >> 4) & 0x0F;
        return num_args >= min_args && num_args <= max_args;
    } else {
        return 0;
    }
}
