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

// Helper
char *parser_str_copy(const char *s) {
    if (!s) return NULL;
    char *d = malloc(strlen(s) + 1);
    if (d) strcpy(d, s);
    return d;
}

LetStatement *parse_let_statement(Parser *p) {
    // 1. Copy current token for AST constructor (since next_token_parser will free it)
    Token letTok = p->cur_token;
    
    Token astLetTok = letTok;
    if (letTok.literal) astLetTok.literal = parser_str_copy(letTok.literal);
    
    if (!expect_peek(p, TOKEN_IDENT)) {
        // Cleanup if failed
        if (astLetTok.literal) free(astLetTok.literal);
        return NULL;
    }
    
    Token identTok = p->cur_token;
    Token astIdentTok = identTok;
    if (identTok.literal) astIdentTok.literal = parser_str_copy(identTok.literal);
    
    Identifier *name = new_identifier(astIdentTok, identTok.literal);
    
    if (!expect_peek(p, TOKEN_ASSIGN)) {
        // Cleanup
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
    if (retTok.literal) astRetTok.literal = parser_str_copy(retTok.literal);

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
