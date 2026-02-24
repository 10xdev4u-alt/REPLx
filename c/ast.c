#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// --- Helper Functions ---
char *ast_str_copy(const char *s) {
    if (!s) return NULL;
    char *d = malloc(strlen(s) + 1);
    if (d) strcpy(d, s);
    return d;
}

// --- Forward Declarations ---
// We need these to assign to function pointers.
// The types in ASTNode are: char *(*token_literal)(struct ASTNode *);
// So our functions must match: char *func(struct ASTNode *node);

char *program_token_literal(struct ASTNode *node);
char *program_string(struct ASTNode *node);
void free_program(struct ASTNode *node);

char *identifier_token_literal(struct ASTNode *node);
char *identifier_string(struct ASTNode *node);
void free_identifier(struct ASTNode *node);

char *let_statement_token_literal(struct ASTNode *node);
char *let_statement_string(struct ASTNode *node);
void free_let_statement(struct ASTNode *node);

char *return_statement_token_literal(struct ASTNode *node);
char *return_statement_string(struct ASTNode *node);
void free_return_statement(struct ASTNode *node);

char *expression_statement_token_literal(struct ASTNode *node);
char *expression_statement_string(struct ASTNode *node);
void free_expression_statement(struct ASTNode *node);


// --- Program ---
void free_program(struct ASTNode *node) {
    Program *p = (Program *)node;
    if (p->statements) {
        for (int i = 0; i < p->statement_count; i++) {
            if (p->statements[i]->free) {
                p->statements[i]->free((ASTNode*)p->statements[i]);
            }
        }
        free(p->statements);
    }
    free(p);
}

char *program_token_literal(struct ASTNode *node) {
    Program *p = (Program *)node;
    if (p->statement_count > 0 && p->statements[0]->token_literal) {
        return p->statements[0]->token_literal((ASTNode*)p->statements[0]);
    }
    return ast_str_copy("");
}

char *program_string(struct ASTNode *node) {
    Program *p = (Program *)node;
    char *out = malloc(1);
    out[0] = '\0';
    
    for (int i = 0; i < p->statement_count; i++) {
        if (p->statements[i]->string) {
            char *s = p->statements[i]->string((ASTNode*)p->statements[i]);
            out = realloc(out, strlen(out) + strlen(s) + 1);
            strcat(out, s);
            free(s);
        }
    }
    return out;
}

Program *new_program() {
    Program *p = malloc(sizeof(Program));
    p->base.type = NODE_PROGRAM;
    p->base.token_literal = program_token_literal;
    p->base.string = program_string;
    p->base.free = free_program;
    p->statement_count = 0;
    p->capacity = 10;
    p->statements = malloc(sizeof(Statement*) * p->capacity);
    return p;
}

void program_append(Program *p, Statement *stmt) {
    if (p->statement_count == p->capacity) {
        p->capacity *= 2;
        p->statements = realloc(p->statements, sizeof(Statement*) * p->capacity);
    }
    p->statements[p->statement_count++] = stmt;
}

// --- Identifier ---
void free_identifier(struct ASTNode *node) {
    Identifier *i = (Identifier *)node;
    // We do NOT free token literal here because tokens are usually passed by value/copy
    // but the literal pointer inside token might be shared or owned.
    // In our lexer, we malloc literal.
    // So we should free it if we own it.
    // But currently token struct is simple value.
    // Let's assume ownership for now to avoid leaks.
    if (i->token.literal) free(i->token.literal);
    if (i->value) free(i->value);
    free(i);
}

char *identifier_token_literal(struct ASTNode *node) {
    Identifier *i = (Identifier *)node;
    return ast_str_copy(i->token.literal);
}

char *identifier_string(struct ASTNode *node) {
    Identifier *i = (Identifier *)node;
    return ast_str_copy(i->value);
}

Identifier *new_identifier(Token token, char *value) {
    Identifier *i = malloc(sizeof(Identifier));
    i->base.type = NODE_IDENTIFIER;
    i->base.token_literal = identifier_token_literal;
    i->base.string = identifier_string;
    i->base.free = free_identifier;
    i->token = token;
    i->value = ast_str_copy(value);
    return i;
}

// --- LetStatement ---
void free_let_statement(struct ASTNode *node) {
    LetStatement *ls = (LetStatement *)node;
    if (ls->token.literal) free(ls->token.literal);
    if (ls->name && ls->name->base.free) ls->name->base.free((ASTNode*)ls->name);
    if (ls->value && ls->value->free) ls->value->free((ASTNode*)ls->value);
    free(ls);
}

char *let_statement_token_literal(struct ASTNode *node) {
    LetStatement *ls = (LetStatement *)node;
    return ast_str_copy(ls->token.literal);
}

char *let_statement_string(struct ASTNode *node) {
    LetStatement *ls = (LetStatement *)node;
    char *out = malloc(1024);
    out[0] = '\0';
    if (ls->token.literal) {
        strcat(out, ls->token.literal);
        strcat(out, " ");
    }
    
    if (ls->name && ls->name->base.string) {
        char *name_str = ls->name->base.string((ASTNode*)ls->name);
        strcat(out, name_str);
        free(name_str);
    }
    
    strcat(out, " = ");
    if (ls->value && ls->value->string) {
        char *val_str = ls->value->string((ASTNode*)ls->value);
        strcat(out, val_str);
        free(val_str);
    }
    strcat(out, ";");
    return out;
}

LetStatement *new_let_statement(Token token) {
    LetStatement *ls = malloc(sizeof(LetStatement));
    ls->base.type = NODE_LET;
    ls->base.token_literal = let_statement_token_literal;
    ls->base.string = let_statement_string;
    ls->base.free = free_let_statement;
    ls->token = token;
    ls->name = NULL;
    ls->value = NULL;
    return ls;
}

// --- ReturnStatement ---
void free_return_statement(struct ASTNode *node) {
    ReturnStatement *rs = (ReturnStatement *)node;
    if (rs->token.literal) free(rs->token.literal);
    if (rs->return_value && rs->return_value->free) rs->return_value->free((ASTNode*)rs->return_value);
    free(rs);
}

char *return_statement_token_literal(struct ASTNode *node) {
    ReturnStatement *rs = (ReturnStatement *)node;
    return ast_str_copy(rs->token.literal);
}

char *return_statement_string(struct ASTNode *node) {
    ReturnStatement *rs = (ReturnStatement *)node;
    char *out = malloc(1024);
    out[0] = '\0';
    if (rs->token.literal) {
        strcat(out, rs->token.literal);
        strcat(out, " ");
    }
    if (rs->return_value && rs->return_value->string) {
        char *val_str = rs->return_value->string((ASTNode*)rs->return_value);
        strcat(out, val_str);
        free(val_str);
    }
    strcat(out, ";");
    return out;
}

ReturnStatement *new_return_statement(Token token) {
    ReturnStatement *rs = malloc(sizeof(ReturnStatement));
    rs->base.type = NODE_RETURN;
    rs->base.token_literal = return_statement_token_literal;
    rs->base.string = return_statement_string;
    rs->base.free = free_return_statement;
    rs->token = token;
    rs->return_value = NULL;
    return rs;
}

// --- ExpressionStatement ---
void free_expression_statement(struct ASTNode *node) {
    ExpressionStatement *es = (ExpressionStatement *)node;
    if (es->token.literal) free(es->token.literal);
    if (es->expression && es->expression->free) es->expression->free((ASTNode*)es->expression);
    free(es);
}

char *expression_statement_token_literal(struct ASTNode *node) {
    ExpressionStatement *es = (ExpressionStatement *)node;
    return ast_str_copy(es->token.literal);
}

char *expression_statement_string(struct ASTNode *node) {
    ExpressionStatement *es = (ExpressionStatement *)node;
    if (es->expression && es->expression->string) {
        return es->expression->string((ASTNode*)es->expression);
    }
    return ast_str_copy("");
}

ExpressionStatement *new_expression_statement(Token token) {
    ExpressionStatement *es = malloc(sizeof(ExpressionStatement));
    es->base.type = NODE_EXPRESSION_STMT;
    es->base.token_literal = expression_statement_token_literal;
    es->base.string = expression_statement_string;
    es->base.free = free_expression_statement;
    es->token = token;
    es->expression = NULL;
    return es;
}
