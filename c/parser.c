#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// Forward Declarations
void next_token_parser(Parser *p);
Statement *parse_statement(Parser *p);
LetStatement *parse_let_statement(Parser *p);
ReturnStatement *parse_return_statement(Parser *p);
ExpressionStatement *parse_expression_statement(Parser *p);
int expect_peek(Parser *p, TokenType t);
void peek_error(Parser *p, TokenType t);

Parser *new_parser(Lexer *l) {
    Parser *p = malloc(sizeof(Parser));
    p->l = l;
    p->error_count = 0;
    p->error_capacity = 10;
    p->errors = malloc(sizeof(char*) * p->error_capacity);
    
    // Read two tokens
    p->cur_token.type = TOKEN_ILLEGAL;
    p->cur_token.literal = NULL;
    p->peek_token.type = TOKEN_ILLEGAL;
    p->peek_token.literal = NULL;

    p->peek_token = next_token(p->l);
    next_token_parser(p);
    
    return p;
}

void free_parser(Parser *p) {
    for (int i = 0; i < p->error_count; i++) {
        free(p->errors[i]);
    }
    free(p->errors);
    if (p->cur_token.literal) free(p->cur_token.literal);
    if (p->peek_token.literal) free(p->peek_token.literal);
    free(p);
}

void next_token_parser(Parser *p) {
    // If we advance, we overwrite cur_token.
    // If cur_token.literal was malloced and not moved to AST, we must free it.
    // However, tracking ownership is hard.
    // Let's assume AST functions COPY the literal string.
    // So Parser ALWAYS frees cur_token.literal before overwriting.
    if (p->cur_token.literal) {
        free(p->cur_token.literal);
    }
    
    p->cur_token = p->peek_token;
    p->peek_token = next_token(p->l);
}

Program *parse_program(Parser *p) {
    Program *program = new_program();
    
    while (p->cur_token.type != TOKEN_EOF) {
        Statement *stmt = parse_statement(p);
        if (stmt) {
            program_append(program, stmt);
        }
        next_token_parser(p);
    }
    return program;
}

Statement *parse_statement(Parser *p) {
    switch (p->cur_token.type) {
        case TOKEN_LET:
            return (Statement*)parse_let_statement(p);
        case TOKEN_RETURN:
            return (Statement*)parse_return_statement(p);
        default:
            return (Statement*)parse_expression_statement(p);
    }
}

LetStatement *parse_let_statement(Parser *p) {
    // 1. Copy current token for AST constructor (since next_token_parser will free it)
    Token letTok = p->cur_token;
    // We must manually duplicate the literal string if AST constructor doesn't.
    // AST constructor (new_let_statement) currently shallow copies Token struct.
    // So letTok.literal points to memory owned by Parser.
    // When parser advances, it frees that memory.
    // So AST has dangling pointer.
    // FIX: We must strdup strictly for AST usage here if AST constructor is shallow.
    // Let's modify AST constructors to strdup inside ast.c? No, let's do it here.
    // But modifying ast.c is cleaner.
    // For now, let's strdup here.
    
    Token astLetTok = letTok;
    if (letTok.literal) astLetTok.literal = strdup(letTok.literal);
    
    if (!expect_peek(p, TOKEN_IDENT)) {
        // Cleanup if failed
        if (astLetTok.literal) free(astLetTok.literal);
        return NULL;
    }
    
    Token identTok = p->cur_token;
    // Same issue, duplicate literal for AST
    // new_identifier duplicates 'value' but shallow copies 'token'.
    // We should duplicate token literal too.
    Token astIdentTok = identTok;
    if (identTok.literal) astIdentTok.literal = strdup(identTok.literal);
    
    Identifier *name = new_identifier(astIdentTok, identTok.literal);
    
    if (!expect_peek(p, TOKEN_ASSIGN)) {
        // Cleanup
        // Identifier destructor frees its token literal and value.
        // So we just free the let token literal we duplicated.
        if (astLetTok.literal) free(astLetTok.literal);
        name->base.free((ASTNode*)name);
        return NULL;
    }
    
    // Skip until semicolon
    while (p->cur_token.type != TOKEN_SEMICOLON && p->cur_token.type != TOKEN_EOF) {
        next_token_parser(p);
    }
    
    LetStatement *stmt = new_let_statement(astLetTok);
    stmt->name = name;
    
    return stmt;
}

ReturnStatement *parse_return_statement(Parser *p) {
    Token retTok = p->cur_token;
    Token astRetTok = retTok;
    if (retTok.literal) astRetTok.literal = strdup(retTok.literal);

    next_token_parser(p);
    
    while (p->cur_token.type != TOKEN_SEMICOLON && p->cur_token.type != TOKEN_EOF) {
        next_token_parser(p);
    }
    
    return new_return_statement(astRetTok);
}

ExpressionStatement *parse_expression_statement(Parser *p) {
    // Placeholder: consume tokens to avoid infinite loop
    // But return NULL for now as we don't have Expressions
    return NULL; 
}

int expect_peek(Parser *p, TokenType t) {
    if (p->peek_token.type == t) {
        next_token_parser(p);
        return 1;
    } else {
        peek_error(p, t);
        return 0;
    }
}

void peek_error(Parser *p, TokenType t) {
    if (p->error_count == p->error_capacity) {
        p->error_capacity *= 2;
        p->errors = realloc(p->errors, sizeof(char*) * p->error_capacity);
    }
    char *msg = malloc(100);
    sprintf(msg, "expected next token to be %d, got %d instead", t, p->peek_token.type);
    p->errors[p->error_count++] = msg;
}
