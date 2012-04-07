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

#include "lexer.h"
#include <stdio.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Creates a new lexer that will read from 'file'.  'env' contains the C
 * lexical environment (unused for now) */
cc_lexer * cc_lexer_init(cc_env * env, char const * file) {
    cc_lexer * self = calloc(1, sizeof(cc_lexer));
    self->in = fopen(file, "r");
    self->ch = fgetc(self->in);
    self->env = env;
    return self; 
}

/* Parses the next token and stores it in self->token.  Current handles only
 * decimal digits, strings, keywords, identifiers, and C-style comments. */
void cc_lexer_next(cc_lexer * self) {
    int c;
restart:
    c = self->ch;
    if ('\n' == c) {
        self->line++;   
        cc_lexer_getc(self);
        goto restart;
    } else if (isspace(c)) {
        cc_lexer_getc(self);
        goto restart;
    }

    if (EOF == self->ch) {
        self->token = CC_TOK_EOF;
        self->value[0] = '\0';
    } else if (isalpha(c) || '_' == c) {
        cc_lexer_id(self);
    } else if (isdigit(c)) {
        cc_lexer_number(self);
    } else if ('"' == c) {
        cc_lexer_string(self);
    } else if ('/' == c) {
        cc_lexer_getc(self);
        if ('*' == self->ch) {
            cc_lexer_comment(self);
            goto restart;
        } else {
            self->token = c;
            self->value[0] = c;
            self->value[1] = '\0';
        }
    } else if ('|' == c) {
        cc_lexer_getc(self);        
        if ('|' == self->ch) {
            self->token = CC_TOK_OR;
        } else {
            ungetc(self->ch, self->in);
            self->token = c;
        }

    } else if ('&' == c) {
        cc_lexer_getc(self);
        if ('&' == self->ch) {
            self->token = CC_TOK_AND;
        } else {
            ungetc(self->ch, self->in);
            self->token = c;
        }
    } else if ('=' == c) {
        cc_lexer_getc(self);
        if ('=' == self->ch) {
            self->token = CC_TOK_EQ;
        } else if ('!' == self->ch) {
            self->token = CC_TOK_NE;
        } else {
            ungetc(self->ch, self->in);
            self->token = c;
        }
    } else {
        self->token = c;
        self->value[0] = c;
        self->value[1] = '\0';
        cc_lexer_getc(self);
    }
}

/* Reads until the next '*', '/' sequence, consuming all characters that are
 * part of the comment. */
void cc_lexer_comment(cc_lexer * self) {
    int c = self->ch;
    while (EOF != self->ch) {
        cc_lexer_getc(self);
        if ('*' == c && '/' == self->ch) {
            cc_lexer_getc(self);
            return;
        } 
        c = self->ch;
    }
}

/* Reads a sequence of digits into an integer literal.  This function will give
 * up if the literal is greater than the size of the literal buffer.  This
 * function expects the next character in the input stream to be a digit.
 * Doesn't handle suffixes (e.g., 32L) */
void cc_lexer_number(cc_lexer * self) {
    int i = 0;
    while(isdigit(self->ch)) {
        if (cc_lexer_checkval(self, i)) {
            cc_lexer_getc(self);
            break;
        }
        self->value[i++] = self->ch;
        cc_lexer_getc(self);
    }
    self->value[i++] = '\0';
    self->token = CC_TOK_NUMBER;
}

/* Reads until the next '"'.  Also reads escape characters '\'.  This function
 * will give up if the literal is greater than the size of the literal buffer.
 * The next character in the input stream should be a '"' to start.
 */
void cc_lexer_string(cc_lexer * self) {
    int i = 0;
    cc_lexer_getc(self); /* Skip first '"' */

    while(EOF != self->ch) {
        if (self->ch == '"') {
            cc_lexer_getc(self);
            break;
        }
        if (cc_lexer_checkval(self, i)) {
            cc_lexer_getc(self);
            break;
        }

        self->value[i++] = self->ch;
        if (self->ch == '\\') {
            self->value[i++] = fgetc(self->in);
        } 
        cc_lexer_getc(self);
    }
    self->value[i++] = '\0';
    self->token = CC_TOK_STRING;
}

/* Parses an identifier [A-Za-z0-9_].  Depending on the value of the
 * identifier, this may actually be a reserved word; in that case, self->token
 * is set to the  token corresponding to the keyword. */
void cc_lexer_id(cc_lexer * self) {
    int i = 0;
    while (isalnum(self->ch) || self->ch == '_') {
        self->value[i++] = self->ch;     
        cc_lexer_getc(self);
        if (cc_lexer_checkval(self, i)) {
            break;
        }
    }
    self->value[i++] = '\0';

    if (!strcmp("if", self->value)) {
        self->token = CC_TOK_IF; 
    } else if (!strcmp("while", self->value)) {
        self->token = CC_TOK_WHILE;
    } else if (!strcmp("return", self->value)) {
        self->token = CC_TOK_RETURN;
    } else if (!strcmp("do", self->value)) {
        self->token = CC_TOK_DO;
    } else if (!strcmp("for", self->value)) {
        self->token = CC_TOK_FOR;
    } else if (!strcmp("int", self->value)) {
        self->token = CC_TOK_INT;
    } else if (!strcmp("char", self->value)) {
        self->token = CC_TOK_CHAR;
    } else if (!strcmp("struct", self->value)) {
        self->token = CC_TOK_STRUCT;
    } else {
        self->token = CC_TOK_ID;
    }
}

/* Checks if the value is too large; if it is, sets the error flag, returns 0,
 * and prints an error message. */
int cc_lexer_checkval(cc_lexer * self, int i) {
    if (i >= sizeof(self->value)-1) {
        fprintf(stderr, "%d: Warning, literal is too large\n", self->line);
        self->errors++;
        return 1;
    } else {
        return 0;
    }
}

/* Gets the next character and stores it in self->ch */
void cc_lexer_getc(cc_lexer * self) {
    self->ch = fgetc(self->in);
}
