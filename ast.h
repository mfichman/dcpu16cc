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

#ifndef CC_AST_H
#define CC_AST_H

/* Used to identify the type of AST node, as declared below. */
typedef enum cc_asttype {
    CC_FUNC,
    CC_BLOCK,
    CC_MEMBER,
    CC_BINARYOP,
    CC_UNARYOP,
    CC_CALL,
    CC_IF,
    CC_FOR,
    CC_WHILE,
    CC_SIMPLE
} cc_asttype;

static int const CC_TYPE_PTR = 1;
static int const CC_TYPE_ARRAY = 2;

/* Base AST node type.  'type' defines the extended type.  'next' is used to
 * join nodes together in a linked list. */
typedef struct cc_astnode {
    cc_asttype type;
    int line; /* Line number of the source text */
    struct cc_astnode * next;
} cc_astnode;

/* Identifier.  These are cached in a global identifier table */
typedef struct cc_id {
    struct cc_id * next;
    char const * str;
} cc_id;

/* C type.  'flags' is used to identify pointers.  If the type is a pointer,
 * then 'type' defines the 'nested' type definition */
typedef struct cc_type {
    int line;
    int flags; 
    struct cc_type * nested;
    cc_id * id;
} cc_type;

typedef struct cc_expr {
    cc_astnode node;
    cc_type * type;
    cc_id * id;
    struct cc_expr * args;
} cc_expr;

/* Used for all variables, local and global */
typedef struct cc_var {
    cc_astnode node;
    cc_expr * init; /* Initializer expr */
    cc_type * type;
    cc_id * id;
    struct cc_var * next;
} cc_var;

/* C statement.  'expr' may be a list if the statement is a block. */
typedef struct cc_stmt {
    cc_astnode node;
    cc_expr * parent;
    struct cc_stmt * next;
} cc_stmt;

/* Simple C statement with a single expression */
typedef struct cc_simple {
    cc_stmt node;
    cc_expr * expr;
} cc_simple;

typedef struct cc_block {
    cc_stmt node;
    cc_var * vars; /* A block may begin with some variables */
    cc_stmt * stmts; 
} cc_block;

typedef struct cc_if {
    cc_stmt node;
    cc_expr * guard;
    cc_stmt * yes;
    cc_stmt * no;
} cc_if;

/* Used for all loop types.  If it's a while or do-while, then 'init' and
 * 'update' will be empty. */
typedef struct cc_loop {
    cc_stmt * node;
    cc_expr * init;
    cc_expr * guard;
    cc_expr * update;
    cc_block * block;
} cc_loop;


/* Struct definition.  The 'var' field is a linked list of attributes of the
 * struct. */
typedef struct cc_struct {
    cc_astnode node;
    cc_var * vars; 
} cc_struct;

/* Struct member dereference. 'left' is the expression to the left of the '.'
 * operator */
typedef struct cc_member {
    cc_expr node;
    cc_id * id;
    cc_expr * left;
} cc_member;

typedef struct cc_call {
    cc_expr node;
    cc_id * id;
    cc_expr * args;
} cc_call;

/* Stores formal function parameters */
typedef struct cc_formal {
    cc_type * type;
    cc_id * id;
    struct cc_formal * next;
} cc_formal;

typedef struct cc_func {
    cc_astnode node;
    cc_id * id;
    cc_formal * formals;
    cc_type * type;
    cc_block * block;
} cc_func;

cc_func* cc_find_func(cc_func* func, cc_id * id);
cc_var* cc_find_var(cc_var* var, cc_id * id);

#endif
