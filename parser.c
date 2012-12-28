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

#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Creates a new lexer that will store the AST in 'env'. */
cc_parser * cc_parser_init(cc_env * env, char const * file) {
    cc_parser * self = calloc(1, sizeof(cc_parser));
    self->env = env;
    self->lexer = cc_lexer_init(env, file);
    cc_lexer_next(self->lexer);
    return self;
}

/* Parses the next global (either a function or global variable).  Since
 * globals and functions have the same prefix of tokens, this function parses
 * the prefix first and then parses either the remainder of the function
 * def/decl or the remainder of the variable def. */
void cc_parser_global(cc_parser * self) {
   
    cc_type * type = cc_parser_type(self);
    cc_id * id = cc_parser_id(self); 
    
    if ('(' == self->lexer->token) {
        /* Parse a function forward declaration or definition */
        cc_func * func = cc_parser_func(self, type, id);
        if (self->env->funcs) {
            func->next = self->env->funcs;
        }
        self->env->funcs = func;
        
    } else if (';' == self->lexer->token) {
        assert(!"Not implemented");
    } else if ('=' == self->lexer->token) {
        assert(!"Not implemented");
    }
}

/* Parses a function.  This function assumes that the type and name of the
 * function have already been parsed. */
cc_func * cc_parser_func(cc_parser * self, cc_type * type, cc_id * id) {
    cc_func * func = calloc(1, sizeof(cc_func));
    cc_formal * formal = 0;

    func->node.type = CC_FUNC;
    func->node.line = self->lexer->line;
    func->type = type;
    func->id = id;
    
    cc_lexer_next(self->lexer); /* Consume '(' */
    while (')' != self->lexer->token && CC_TOK_EOF != self->lexer->token) {
        if (!formal) {
            func->formals = formal = cc_parser_formal(self);
        } else {
            formal->next = cc_parser_formal(self);
            formal = formal->next;
        } 
        if (')' != self->lexer->token && ',' != self->lexer->token) {
            cc_parser_err(self, self->lexer->line, "Expected ',' or ')'");
            break;
        } else if (',' == self->lexer->token) {
            cc_lexer_next(self->lexer); /* Consume ',' */
        }
    }
    cc_lexer_next(self->lexer);

    if (';' == self->lexer->token) {
        /* Forward declaration */
        cc_lexer_next(self->lexer);
    } else if ('{' == self->lexer->token) {
        func->block = cc_parser_block(self);
    } else if (CC_TOK_EOF == self->lexer->token) {
        cc_parser_err(self, self->lexer->line, "Unexpected EOF");
    } else {
        cc_parser_err(self, self->lexer->line, "Unexpected token");
    }
    return func;
}

/* Parses a type.  Types in C are a bit complicated: you can essentially have
 * 'nested types' inside of a single type in the source code.  For example, you
 * can have a 'struct foo', but also a 'struct foo *' or a 'struct foo **'.
 * So, this function recursively parses each part.  Note that since the
 * compiler does not support typedefs, things are a bit more simple.  We can
 * always identify a type right off the bat, because it will start with 'int',
 * 'char' or 'struct'.  If typedefs are implemented, then a symbol table lookup
 * would be needed to disambiguate the * operator (i.e., a * b could be an
 * expression or a definition). */
cc_type * cc_parser_type(cc_parser * self) {
    cc_type * type = calloc(1, sizeof(cc_type)); 
    type->line = self->lexer->line;

    /* Note: This doesn't handle function pointers yet. */
    
    if (CC_TOK_INT == self->lexer->token) {
        type->id = cc_env_id(self->env, "int");   
    } else if (CC_TOK_CHAR == self->lexer->token) {
        type->id = cc_env_id(self->env, "char");
    } else if (CC_TOK_STRUCT == self->lexer->token) {
        cc_lexer_next(self->lexer);
        if (CC_TOK_ID != self->lexer->token) {
            cc_parser_err(self, self->lexer->line, "Expected an identifier");
        } else {
            type->id = cc_env_id(self->env, self->lexer->value);
        }
    } 
    cc_lexer_next(self->lexer);

    while (1) {
        if ('*' == self->lexer->token) {
            cc_type * temp = calloc(1, sizeof(cc_type));
            temp->flags |= CC_TYPE_PTR;
            temp->nested = type;
            type = temp; 
            cc_lexer_next(self->lexer);
        } else if ('[' == self->lexer->token) {
            cc_type * temp = calloc(1, sizeof(cc_type));
            temp->flags |= CC_TYPE_ARRAY;
            temp->nested = type;
            type = temp; 
            cc_lexer_next(self->lexer);
            if (']' != self->lexer->token) {
                cc_parser_err(self, self->lexer->line, "Unexpected token");
            }
            cc_lexer_next(self->lexer);
        } else {
            break;
        }
    } 

    return type;
}

/* Parses a formal parameter to a function definition or declaration. */
cc_formal * cc_parser_formal(cc_parser * self) {
    cc_formal * formal = calloc(1, sizeof(cc_formal));
    formal->type = cc_parser_type(self);
    formal->id = cc_parser_id(self);
    return formal;
}

/* Parses an identifier.  If the next token is not an identifier, then this
 * function will set the error flag and return the 'empty' identifier. */
cc_id * cc_parser_id(cc_parser * self) {

    if (CC_TOK_ID != self->lexer->token) {
        cc_parser_err(self, self->lexer->line, "Expected an identifier");
        return cc_env_id(self->env, "");
    } else {
        cc_id * id = cc_env_id(self->env, self->lexer->value);
        cc_lexer_next(self->lexer);
        return id;
    }
}

/* Parses a block statement. */
cc_block * cc_parser_block(cc_parser * self) {
    cc_block * block = calloc(1, sizeof(cc_block));
    cc_stmt * stmt = 0;
    cc_var * var = 0;
    block->node.node.line = self->lexer->line;
    block->node.node.type = CC_BLOCK;

    if ('{' != self->lexer->token) {
        cc_parser_err(self, self->lexer->line, "Expected '{'");
    } 
    cc_lexer_next(self->lexer);

    /* Parse the variable declarations.  The compiler only supports C90, so
     * the variable declarations must be at the beginning of the block. */
    while ((CC_TOK_INT == self->lexer->token
        || CC_TOK_CHAR == self->lexer->token
        || CC_TOK_STRUCT == self->lexer->token)
        && ('}' != self->lexer->token)
        && (CC_TOK_EOF != self->lexer->token)) {

        if (var) {
            var->next = cc_parser_var(self);
            var = var->next;
        } else {
            block->vars = var = cc_parser_var(self);
        }
    }

    while ('}' != self->lexer->token && CC_TOK_EOF != self->lexer->token) {
    
        /* Parse statements inside the block */
        if (stmt) {
            stmt->next = cc_parser_stmt(self);
            stmt = stmt->next;
        } else {
            block->stmts = stmt = cc_parser_stmt(self);
        }
    } 
    cc_lexer_next(self->lexer); /* Consume '}' */

    return block;
}

/* Parses a local variable definition, including the intiailizer expression if
 * present. */
cc_var * cc_parser_var(cc_parser * self) {
    cc_var * var = calloc(1, sizeof(cc_var));
    var->node.line = self->lexer->line;
    var->node.type = CC_VAR;
    var->type = cc_parser_type(self);
    var->id = cc_parser_id(self);

    if ('=' == self->lexer->token) {
        cc_lexer_next(self->lexer);
        var->init = cc_parser_expr(self);
        if (';' != self->lexer->token) {
            cc_parser_err(self, self->lexer->line, "Missing ';'");
        }
        cc_lexer_next(self->lexer);
    } else if (';' == self->lexer->token) {
        cc_lexer_next(self->lexer);
    } else {
        cc_parser_err(self, self->lexer->line, "Expected '=' or ';'");
    }

    return var;
}

cc_stmt * cc_parser_stmt(cc_parser * self) {
    if (CC_TOK_FOR == self->lexer->token) {
        return (cc_stmt *)cc_parser_for(self);
    } else if (CC_TOK_WHILE == self->lexer->token) {
        return (cc_stmt *)cc_parser_while(self);
    } else if (CC_TOK_IF == self->lexer->token) {
        return (cc_stmt *)cc_parser_if(self);
    } else if (CC_TOK_RETURN == self->lexer->token) {
        return (cc_stmt *)cc_parser_return(self);
    } else {
        
        cc_simple * stmt = calloc(1, sizeof(cc_simple));
        stmt->node.node.line = self->lexer->line;
        stmt->node.node.type = CC_SIMPLE; 
        stmt->expr = cc_parser_expr(self);
        cc_lexer_next(self->lexer);
        return (cc_stmt *)stmt;
    }
}

cc_return * cc_parser_return(cc_parser * self) {
    cc_return * ret = calloc(1, sizeof(cc_return));
    ret->node.node.line = self->lexer->line;
    ret->node.node.type = CC_RETURN;
    cc_lexer_next(self->lexer);
    if (';' != self->lexer->token) {
        ret->expr = cc_parser_expr(self);
    }
    if (';' != self->lexer->token) {
        cc_parser_err(self, self->lexer->line, "Missing ';'");
    }
    cc_lexer_next(self->lexer);
    return ret;
}

cc_if * cc_parser_if(cc_parser * self) {

    return 0;
}

/* Parses a for loop.  Only C90 is supported, so no variables may be declared
 * in the initialization expression of the for loop. */
cc_loop * cc_parser_for(cc_parser * self) {
    cc_loop * loop = calloc(1, sizeof(cc_loop));
    loop->node.node.line = self->lexer->line;
    loop->node.node.type = CC_FOR;
    cc_lexer_next(self->lexer);
    if ('(' != self->lexer->token) {
        cc_parser_err(self, self->lexer->line, "Expected '('");
    }
    cc_lexer_next(self->lexer); 

    if (';' != self->lexer->token) {
        loop->init = cc_parser_expr(self); 
    }
    if (';' != self->lexer->token) {
        cc_parser_err(self, self->lexer->line, "Expected ';'");
    }
    cc_lexer_next(self->lexer);
     
    if (';' != self->lexer->token) {
        loop->guard = cc_parser_expr(self);
    } 
    if (';' != self->lexer->token) {
        cc_parser_err(self, self->lexer->line, "Expected ';'");
    }
    cc_lexer_next(self->lexer);
    
    if (')' != self->lexer->token) {
        loop->update = cc_parser_expr(self);
    } 

    if (')' != self->lexer->token) {
        cc_parser_err(self, self->lexer->line, "Expected ')'");
    }
    cc_lexer_next(self->lexer);

    if (';' != self->lexer->token) {
        loop->block = cc_parser_block(self);
    }
    return loop;
}

cc_loop * cc_parser_while(cc_parser * self) {
    return 0;
}

cc_expr * cc_parser_expr(cc_parser * self) {
    return cc_parser_expr2(self, 0);
}

/* Parses an expression.   This parser uses a precedence parsing table to parse
 * binary expressions, and then calls out to the unary expression parser for
 * the base case. */
cc_expr * cc_parser_expr2(cc_parser * self, int i) {
    static cc_token table[] = { '=', CC_TOK_OR, CC_TOK_AND, '|', '&', CC_TOK_EQ, 
        CC_TOK_NE, '<', '>', CC_TOK_LE, CC_TOK_GE, CC_TOK_RSHIFT, 
        CC_TOK_LSHIFT, '+', '-', '*', '/', '%'
    };
    cc_token token = table[i];
    cc_expr * expr = 0;

    if (i * sizeof(cc_token) > sizeof(table)) {
        return cc_parser_unary(self);
    }

    expr = cc_parser_expr2(self, i+1);
    while (token == self->lexer->token) {
        cc_binary * binary = calloc(1, sizeof(cc_binary)); 
        binary->node.node.line = self->lexer->line;
        binary->node.node.type = CC_BINARY;
        binary->op = token;
        binary->left = expr;
        cc_lexer_next(self->lexer);
        binary->right = cc_parser_expr2(self, i+1);
        expr = (cc_expr *)binary;
    }
    return expr;
}

/* Parses a unary expression. */
cc_expr * cc_parser_unary(cc_parser * self) {
    if ('+' == self->lexer->token) {
        cc_lexer_next(self->lexer);
        return cc_parser_call(self);
    } else if ('-' == self->lexer->token || '~' == self->lexer->token
        || '!' == self->lexer->token || '*' == self->lexer->token
        || '&' == self->lexer->token) {

        cc_unary * unary = calloc(1, sizeof(cc_unary));
        unary->node.node.line = self->lexer->line;
        unary->node.node.type = CC_UNARY;
        unary->op = self->lexer->token;
        cc_lexer_next(self->lexer);
        unary->expr = cc_parser_unary(self);
        return (cc_expr *)unary; 
    } else {
        return cc_parser_call(self);
    }
}

/* Parse a call expression.  Any expression can end with '('.  If the
 * expression is an identifier, then this is simply a direct call to a
 * function.  Otherwise, it's a call via a function pointer. */
cc_expr * cc_parser_call(cc_parser * self) {
    cc_expr * expr = cc_parser_member(self);
    while ('(' == self->lexer->token) {
        cc_call * call = calloc(1, sizeof(cc_call));
        cc_expr * arg = 0;
        call->node.node.line = self->lexer->line;
        call->node.node.type = CC_CALL;
        call->expr = expr;
        cc_lexer_next(self->lexer);
        while (')' != self->lexer->token && CC_TOK_EOF != self->lexer->token) {
            if (arg) { 
                arg->next = cc_parser_expr(self);
                arg = arg->next;
            } else {
                call->args = arg = cc_parser_expr(self);
            }
            if (')' != self->lexer->token && ',' != self->lexer->token) {
                cc_parser_err(self, self->lexer->line, "Expected ',' or ')'");
                break;
            } else if (',' == self->lexer->token) {
                cc_lexer_next(self->lexer);
            }
        }
        cc_lexer_next(self->lexer); /* Consume ')' */
        expr = (cc_expr *)call;
    }
    return expr;
}

/* Parses a member access expression.  This includes x.y, x->y */
cc_expr * cc_parser_member(cc_parser * self) {
    cc_expr * expr = cc_parser_ref(self);
    while ('.' == self->lexer->token || CC_TOK_ARROW == self->lexer->token) {
        cc_member * member = calloc(1, sizeof(cc_binary));
        member->node.node.line = self->lexer->line;
        member->node.node.type = CC_BINARY;
        cc_lexer_next(self->lexer);
        if (CC_TOK_ARROW == self->lexer->token) {
            /* Translate x->y into  (*x)->y */
            cc_unary * unary = calloc(1, sizeof(cc_unary));
            unary->node.node.line = self->lexer->line;
            unary->node.node.type = CC_UNARY;
            unary->op = '*';
            unary->expr = expr;
            expr = (cc_expr *)unary;
        }
        member->expr = expr; 
        member->id = cc_parser_id(self);
        expr = (cc_expr *)member;
    }
    return expr;
}

/* Identifier reference (a.k.a. variable access) */
cc_expr * cc_parser_ref(cc_parser * self) {
    if (CC_TOK_ID == self->lexer->token) {
        cc_ref * ref = calloc(1, sizeof(cc_ref));
        ref->node.node.line = self->lexer->line;
        ref->node.node.type = CC_REF;
        ref->id = cc_parser_id(self);
        return (cc_expr *)ref;
    }
    return cc_parser_string(self);
}

cc_expr * cc_parser_string(cc_parser * self) {
    if (CC_TOK_STRING == self->lexer->token) {
        cc_string * string = calloc(1, sizeof(cc_string));
        string->node.node.line = self->lexer->line;
        string->node.node.type = CC_STRING;
        string->value = strdup(self->lexer->value); 
        cc_lexer_next(self->lexer);
        return (cc_expr *)string;
    }
    return cc_parser_number(self);
}

cc_expr * cc_parser_number(cc_parser * self) {
    if (CC_TOK_NUMBER == self->lexer->token) {
        cc_number * number = calloc(1, sizeof(cc_number));
        number->node.node.line = self->lexer->line;
        number->node.node.type = CC_NUMBER;
        number->value = strdup(self->lexer->value);
        cc_lexer_next(self->lexer);
        return (cc_expr *)number;
    } else {
        cc_parser_err(self, self->lexer->line, "Expected an expression");
        cc_lexer_next(self->lexer);
        return 0;
    }
}



void cc_parser_err(cc_parser * self, int line, char const * msg) {
    fprintf(stderr, "%d: %s\n", line, msg);
    self->errors++;
}
