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

#include "env.h"
#include "lexer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int tabs = 0;

/* Does a naive linear search through the list of identifiers to see if there's
 * already a matching one */
cc_id * cc_env_id(cc_env * self, char const * str) {
    cc_id * id = 0;
    for (id = self->ids; id; id = id->next) {
        if (!strcmp(id->str, str)) {
            return id;
        }
    } 
    id = calloc(1, sizeof(cc_id));
    id->str = strdup(str);  
    return id;
}

void cc_env_print(cc_env * self) {
    cc_func * func = 0;
    for (func = self->funcs; func; func = func->next) {
        cc_func_print(func); 
    }
    fflush(stdout);
}

void cc_func_print(cc_func * self) {
    cc_formal * form = 0;
    cc_type_print(self->type);
    putc(' ', stdout);
    cc_id_print(self->id);
    putc('(', stdout);

    for (form = self->formals; form; form = form->next) {
        cc_formal_print(form);
        if (form->next) {
            printf(", ");
        }
    }
    cc_block_print(self->block);
}

void cc_formal_print(cc_formal * formal) {
    cc_type_print(formal->type);
    putc(' ', stdout);
    cc_id_print(formal->id);
}

void cc_id_print(cc_id * id) {
    printf("%s", id->str);
}

void cc_type_print(cc_type * self) {
    if (self->nested) {
        cc_type_print(self->nested);
    }
    if (self->flags & CC_TYPE_PTR) {
        printf("*");
    }
    else if (self->flags & CC_TYPE_ARRAY) {
        printf("[]");
    }
    else {
        printf("%s", self->id->str);
    }
}

void cc_block_print(cc_block * self) {
    cc_var * var = 0;
    cc_stmt * stmt = 0;
    int i = 0;

    printf(") {\n");
    tabs++;
    for (var = self->vars; var; var = var->next) {
        cc_var_print(var);
    }
    for (stmt = self->stmts; stmt; stmt = stmt->next) {
        cc_stmt_print(stmt);
    }
    tabs--;
    
    for (i = 0; i < tabs; ++i) {
        printf("    ");
    }
    printf("}\n\n");
}

void cc_var_print(cc_var * var) {
    int i = 0;
    for (i = 0; i < tabs; ++i) {
        printf("    ");
    }
    cc_type_print(var->type); 
    putc(' ', stdout);
    cc_id_print(var->id);
    printf(" = ");
    cc_expr_print(var->init); 
    printf(";\n");
}

void cc_stmt_print(cc_stmt * self) {
    switch (self->node.type) {
    case CC_IF:
        cc_if_print((cc_if *)self);
        break;
    case CC_FOR:
        cc_for_print((cc_loop *)self);
        break;
    case CC_WHILE:
        cc_while_print((cc_loop *)self);
        break;
    case CC_SIMPLE:
        cc_simple_print((cc_simple *)self);
        break;
    case CC_RETURN:
        cc_return_print((cc_return *)self);
        break;
    default:
        fprintf(stderr, "Invalid expression code\n");
        break;
    }
}

void cc_if_print(cc_if * self) {
}

void cc_for_print(cc_loop * self) {
    int i = 0;
    for (i = 0; i < tabs; ++i) {
        printf("    ");
    }
    printf("for (");
    if (self->init) {
        cc_expr_print(self->init);
    }
    putc(';', stdout);
    if (self->guard) {
        cc_expr_print(self->guard); 
    }
    putc(';', stdout);
    if (self->update) {
        cc_expr_print(self->update);
    }
    printf(")");
    if (self->block) {
        cc_block_print(self->block);
    } else {
        printf(";\n");
    }
}

void cc_while_print(cc_loop * self) {
}

void cc_simple_print(cc_simple * self) {
    int i = 0;
    for (i = 0; i < tabs; ++i) {
        printf("    ");
    }
    cc_expr_print(self->expr);
    printf(";\n");
}

void cc_return_print(cc_return * self) {
    int i = 0;
    for (i = 0; i < tabs; ++i) {
        printf("    ");
    }
    if (self->expr) {
        printf("return ");
        cc_expr_print(self->expr);
    } else {
        printf("return");
    }
    printf(";\n");
    

}

void cc_expr_print(cc_expr * self) {
    switch (self->node.type) {
    case CC_MEMBER:
        cc_member_print((cc_member *)self);
        break;
    case CC_BINARY:
        cc_binary_print((cc_binary *)self);
        break;
    case CC_UNARY:
        cc_unary_print((cc_unary *)self);
        break;
    case CC_CALL:
        cc_call_print((cc_call *)self);
        break;
    case CC_REF:
        cc_ref_print((cc_ref *)self);
        break;
    case CC_NUMBER:
        cc_number_print((cc_number *)self);
        break;
    case CC_STRING:
        cc_string_print((cc_string *)self);
        break;
    default:
        fprintf(stderr, "Invalid expression code\n");
        break;
    }
}

void cc_member_print(cc_member * self) {
}

void cc_binary_print(cc_binary * self) {
    putc('(', stdout);
    cc_expr_print(self->left);
    if (self->op < 255) {
        printf(" %c ", (char)self->op);
    } else if (CC_TOK_AND == self->op) {
        printf(" && ");
    } else if (CC_TOK_OR == self->op) {
        printf(" || ");
    } else if (CC_TOK_EQ == self->op) {
        printf(" == ");
    } else if (CC_TOK_NE == self->op) {
        printf(" != ");
    } else if (CC_TOK_LE == self->op) {
        printf(" <= ");
    } else if (CC_TOK_GE == self->op) {
        printf(" >= ");
    } else if (CC_TOK_RSHIFT == self->op) {
        printf(" >> ");
    } else if (CC_TOK_LSHIFT == self->op) {
        printf(" << ");
    } else if (CC_TOK_ARROW == self->op) {
        printf("->");
    }
    cc_expr_print(self->right);
    putc(')', stdout);
}

void cc_unary_print(cc_unary * self) {
    putc((char)self->op, stdout);
    cc_expr_print(self->expr);
}

void cc_call_print(cc_call * self) {
    cc_expr * arg = 0;
    cc_expr_print(self->expr);
    putc('(', stdout);
    for (arg = self->args; arg; arg = arg->next) {
        cc_expr_print(arg); 
        if (arg->next) {
            printf(", ");
        }
    }
    putc(')', stdout);
}

void cc_ref_print(cc_ref * self) {
    cc_id_print(self->id);
}

void cc_number_print(cc_number * self) {
    printf("%s", self->value);
}

void cc_string_print(cc_string * self) {
    printf("%s", self->value);
}

