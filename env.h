/*
 * Copyright (c) 2012 Matt Fichman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, APEXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */  

#ifndef CC_ENV_H
#define CC_ENV_H

#include "ast.h"

/* The environment holds the global symbol table,
 * amongst other things.  Any shared whole-program
 * state should go here */
typedef struct cc_env {
    cc_func * funcs;
    cc_var * vars; /* Global variables */
    cc_id * ids; /* Identifiers */
} cc_env;

cc_id * cc_env_id(cc_env * self, char const * str);
cc_var * cc_env_var(cc_env * self, cc_id * id);
cc_func * cc_env_func(cc_env * self, cc_id * id);
void cc_env_print(cc_env * self);
void cc_func_print(cc_func * self);
void cc_type_print(cc_type * self);
void cc_formal_print(cc_formal * self);
void cc_id_print(cc_id * id);

#endif
