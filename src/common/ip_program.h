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

#ifndef INTERPROGRAM_PROGRAM_H
#define INTERPROGRAM_PROGRAM_H

#include "ip_ast.h"
#include "ip_labels.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure of a program in memory after it has been parsed.
 */
typedef struct
{
    /** Table containing all variables in the program */
    ip_var_table_t vars;

    /** Table containing all labels in the program */
    ip_label_table_t labels;

    /** List of all statements in the program */
    ip_ast_list_t statements;

    /** Name of the file that the program was loaded from */
    char *filename;

} ip_program_t;

/**
 * @brief Creates a new program.
 *
 * @param[in] filename Name of the file the program was loaded from.
 */
ip_program_t *ip_program_new(const char *filename);

/**
 * @brief Frees a program.
 *
 * @param[int] program The program state to free.
 */
void ip_program_free(ip_program_t *program);

#ifdef __cplusplus
}
#endif

#endif
