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

/* List of reserved words an other token types.  Individual character operators
 * have a token code equal to their ASCII code extended to 4 bytes */
enum cc_token {
    CC_TOK_IF = 255, /* Larger than any character */
    CC_TOK_ELSE,
    CC_TOK_WHILE,
    CC_TOK_RETURN,
    CC_TOK_DO,
    CC_TOK_INT,
    CC_TOK_CHAR,
    CC_TOK_STRUCT, 
};

typedef struct cc_lexer {
    cc_env * env; 
    cc_token token[16]; /* Array of tokens for lookahead */
    int tokenind; /* Index of the current token */
    char value[1024]; /* Nothing longer than this */
    int errors;
};

cc_lexer* cc_lexer_init(const char* file);
void cc_lexer_next();
void cc_lexer_comment();
void cc_lexer_number();
void cc_lexer_string();
void cc_lexer_id();
void cc_lexer_read();
cc_token cc_lexer_token();

#endif
