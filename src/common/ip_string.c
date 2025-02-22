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

#include "ip_string.h"
#include "ip_types.h"
#include <stdlib.h>
#include <string.h>

ip_string_t *ip_string_create(const char *str)
{
    if (str && *str != '\0') {
        return ip_string_create_with_length(str, strlen(str));
    } else {
        return ip_string_create_empty();
    }
}

ip_string_t *ip_string_create_with_length(const char *str, size_t len)
{
    ip_string_t *nstr;
    if (len == 0) {
        return ip_string_create_empty();
    }
    nstr = malloc(sizeof(ip_string_t) + len);
    if (!nstr) {
        ip_out_of_memory();
    }
    nstr->ref = 1;
    nstr->len = len;
    memcpy(nstr->data, str, len);
    nstr->data[len] = '\0';
    return nstr;
}

ip_string_t *ip_string_create_empty(void)
{
    /* String object that can never be deallocated because "ref" is 1 */
    static ip_string_t empty = {1, 0, {0}};
    ip_string_ref(&empty);
    return &empty;
}

size_t ip_string_length(const ip_string_t *str)
{
    return str ? str->len : 0;
}

void ip_string_ref(ip_string_t *str)
{
    /* In the future, may want to make this atomic for multi-threaded code */
    if (str) {
        ++(str->ref);
    }
}

void ip_string_deref(ip_string_t *str)
{
    /* In the future, may want to make this atomic for multi-threaded code */
    if (str) {
        if (--(str->ref) == 0) {
            free(str);
        }
    }
}

ip_string_t *ip_string_concat(ip_string_t *str1, ip_string_t *str2)
{
    if (!str1 && !str2) {
        return ip_string_create_empty();
    } else if (!str1 || str1->len == 0) {
        ip_string_ref(str2);
        return str2;
    } else if (!str2 || str2->len == 0) {
        ip_string_ref(str1);
        return str1;
    } else {
        size_t len = str1->len + str2->len;
        ip_string_t *nstr = malloc(sizeof(ip_string_t) + len);
        if (!nstr) {
            ip_out_of_memory();
        }
        nstr->ref = 1;
        nstr->len = len;
        memcpy(nstr->data, str1->data, str1->len);
        memcpy(nstr->data + str1->len, str2->data, str2->len);
        nstr->data[len] = '\0';
        return nstr;
    }
    return 0;
}

ip_string_t *ip_string_substring(ip_string_t *str, size_t start, size_t len)
{
    if (!str) {
        return ip_string_create_empty();
    }
    if (start >= str->len) {
        if (str->len == 0) {
            ip_string_ref(str);
            return str;
        }
        return ip_string_create_empty();
    }
    if (len > (str->len - start)) {
        len = str->len - start;
    }
    if (start == 0 && len == str->len) {
        ip_string_ref(str);
        return str;
    }
    return ip_string_create_with_length(str->data + start, len);
}
