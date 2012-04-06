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
enum cc_asttype {
    CC_FUNC,
    CC_STMT,
    CC_BLOCK,
    CC_MEMBER,
    CC_BINARYOP,
    CC_UNARYOP,
    CC_CALL,
    CC_IF,
    CC_FOR,
    CC_WHILE,
};

/* Base AST node type.  'type' defines the extended type.  'next' is used to
 * join nodes together in a linked list. */
typedef struct cc_astnode {
    cc_asttype type;
    cc_astnode * next;
} cc_astnode;

/* Identifier.  These are cached in a global identifier table */
typedef struct cc_id {
    char * str;
} cc_id;

/* C type.  'flags' is used to identify pointers.  If the type is a pointer,
 * then 'type' defines the 'nested' type definition */
typedef struct cc_type {
    int flags; 
    cc_type * nested;
    cc_id id;
} cc_type;

/* C statement.  'expr' may be a list if the statement is a block. */
typedef struct cc_stmt {
    cc_astnode node;
    cc_var * vars; /* A block may begin with some variables */
    cc_expr * exprs; 
    cc_expr * parent;
} cc_stmt;

/* Struct definition.  The 'var' field is a linked list of attributes of the
 * struct. */
typedef struct cc_struct {
    cc_astnode node;
    cc_var * vars; 
} cc_struct;

typedef struct cc_expr {
    cc_astnode node;
    cc_type * type;
} cc_type;

/* Struct member dereference. 'left' is the expression to the left of the '.'
 * operator */
typedef struct cc_member {
    cc_expr node;
    cc_id id;
    cc_expr * left;
} cc_member;

typedef struct cc_binaryop {
    cc_expr node;
    cc_expr * left;
    cc_expr * right;
} cc_binaryop;

typedef struct cc_unaryop {
    cc_expr node;
    cc_expr * child;
} cc_unaryop;

typedef struct cc_call {
    cc_expr node;
    cc_id id;
    cc_expr * args;
} cc_call;

typedef struct cc_if {
    cc_astnode node;
    cc_expr * guard;
    cc_stmt * yes;
    cc_stmt * no;
} cc_if;

/* Used for all loop types.  If it's a while or do-while, then 'init' and
 * 'update' will be empty. */
typedef struct cc_loop {
    cc_astnode node;
    cc_astnode * init;
    cc_astnode * guard;
    cc_astnode * update;
    cc_stmt * stmt;
} cc_for;

/* Used for all variables, local and global */
typedef struct cc_var {
    cc_astnode node;
    cc_expr * init; /* Initializer expr */
    cc_type * type;
    cc_id id;
} cc_var;

/* Stores formal function parameters */
typedef struct cc_formal {
    cc_type * type;
    cc_id id;
    cc_formal * next;
} cc_formal;

typedef struct cc_func {
    cc_astnode node;
    cc_id id;
    cc_formal * formals;
    cc_type * type;
} cc_func;

cc_func* cc_find_func(cc_func* func, cc_id id);
cc_var* cc_find_var(cc_var* var, cc_id id);

#endif
