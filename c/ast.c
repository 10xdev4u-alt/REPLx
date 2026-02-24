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

// --- Program ---
void free_program(ASTNode *node) {
    Program *p = (Program *)node;
    for (int i = 0; i < p->statement_count; i++) {
        if (p->statements[i]->free) {
            p->statements[i]->free(p->statements[i]);
        }
    }
    free(p->statements);
    free(p);
}

char *program_token_literal(ASTNode *node) {
    Program *p = (Program *)node;
    if (p->statement_count > 0) {
        return p->statements[0]->token_literal(p->statements[0]);
    }
    return "";
}

char *program_string(ASTNode *node) {
    Program *p = (Program *)node;
    // Calculate total length first? Or use realloc.
    // For simplicity, we'll use a fixed buffer and hope (BAD for prod, ok for MVP)
    // Better: dynamic string builder.
    // Let's implement a tiny string builder.
    
    char *out = malloc(1);
    out[0] = '\0';
    
    for (int i = 0; i < p->statement_count; i++) {
        char *s = p->statements[i]->string(p->statements[i]);
        out = realloc(out, strlen(out) + strlen(s) + 1);
        strcat(out, s);
        free(s);
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
void free_identifier(ASTNode *node) {
    Identifier *i = (Identifier *)node;
    free_token(i->token);
    free(i->value);
    free(i);
}

char *identifier_token_literal(ASTNode *node) {
    Identifier *i = (Identifier *)node;
    return i->token.literal;
}

char *identifier_string(ASTNode *node) {
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
void free_let_statement(ASTNode *node) {
    LetStatement *ls = (LetStatement *)node;
    free_token(ls->token);
    if (ls->name) ls->name->base.free((ASTNode*)ls->name);
    if (ls->value) ls->value->free((ASTNode*)ls->value);
    free(ls);
}

char *let_statement_token_literal(ASTNode *node) {
    LetStatement *ls = (LetStatement *)node;
    return ls->token.literal;
}

char *let_statement_string(ASTNode *node) {
    LetStatement *ls = (LetStatement *)node;
    char *out = malloc(1024); // Dangerous fixed size
    strcpy(out, ls->token.literal);
    strcat(out, " ");
    
    char *name_str = ls->name->base.string((ASTNode*)ls->name);
    strcat(out, name_str);
    free(name_str);
    
    strcat(out, " = ");
    if (ls->value) {
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
void free_return_statement(ASTNode *node) {
    ReturnStatement *rs = (ReturnStatement *)node;
    free_token(rs->token);
    if (rs->return_value) rs->return_value->free((ASTNode*)rs->return_value);
    free(rs);
}

char *return_statement_token_literal(ASTNode *node) {
    ReturnStatement *rs = (ReturnStatement *)node;
    return rs->token.literal;
}

char *return_statement_string(ASTNode *node) {
    ReturnStatement *rs = (ReturnStatement *)node;
    char *out = malloc(1024);
    strcpy(out, rs->token.literal);
    strcat(out, " ");
    if (rs->return_value) {
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
void free_expression_statement(ASTNode *node) {
    ExpressionStatement *es = (ExpressionStatement *)node;
    free_token(es->token);
    if (es->expression) es->expression->free((ASTNode*)es->expression);
    free(es);
}

char *expression_statement_token_literal(ASTNode *node) {
    ExpressionStatement *es = (ExpressionStatement *)node;
    return es->token.literal;
}

char *expression_statement_string(ASTNode *node) {
    ExpressionStatement *es = (ExpressionStatement *)node;
    if (es->expression) {
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
