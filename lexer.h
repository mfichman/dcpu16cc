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

#ifndef CC_LEXER_H
#define CC_LEXER_H

#include "env.h"
#include <stdio.h>

/* List of reserved words an other token types.  Individual character operators
 * have a token code equal to their ASCII code extended to 4 bytes */
typedef enum cc_token {
    CC_TOK_IF = 255, /* Larger than any character */
    CC_TOK_ELSE,
    CC_TOK_WHILE,
    CC_TOK_RETURN,
    CC_TOK_DO,
    CC_TOK_FOR,
    CC_TOK_INT,
    CC_TOK_CHAR,
    CC_TOK_STRUCT, 
    CC_TOK_STRING,
    CC_TOK_NUMBER,
    CC_TOK_EOF
} cc_token;

typedef struct cc_lexer {
    cc_env * env; 
    cc_token token; /* Current token */
    char value[1024]; /* No literals longer than this are allowed */
    int errors;
    int line;
    int ch;
    FILE * in;
} cc_lexer;

cc_lexer * cc_lexer_init(cc_env * env, char const * file);
void cc_lexer_next(cc_lexer * self);
void cc_lexer_comment(cc_lexer * self);
void cc_lexer_number(cc_lexer * self);
void cc_lexer_string(cc_lexer * self);
void cc_lexer_id(cc_lexer * self);
int cc_lexer_checkval(cc_lexer * self, int i);
void cc_lexer_getc(cc_lexer * self);
#endif
