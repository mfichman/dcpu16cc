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

#ifndef CC_PARSER_H
#define CC_PARSER_H

typedef struct cc_parser {
    cc_env * env;
    cc_lexer * lexer;
    int errors;
};

cc_parser * cc_parser_init(const char * fname);
cc_func * cc_parser_func(cc_parser * self);
cc_id * cc_parser_id(cc_parser * self);
cc_type * cc_parser_type(cc_parser * self);
cc_stmt * cc_parser_stmt(cc_parser * self);
cc_member * cc_parser_member(cc_parser * self);
cc_binaryop * cc_parser_binaryop(cc_parser * self);
cc_unaryop * cc_parser_unaryop(cc_parser * self);
cc_call * cc_parser_call(cc_parser * self);
cc_if * cc_parser_if(cc_parser * self);
cc_loop * cc_parser_for(cc_parser * self);
cc_loop * cc_parser_while(cc_parser * self);
cc_formal * cc_parser_formal(cc_parser * self);
cc_func * cc_parser_func(cc_parser * self);
void cc_parser_destroy(cc_parser * self);

#endif
