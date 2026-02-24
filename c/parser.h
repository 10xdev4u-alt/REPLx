#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include "token.h"

typedef struct {
    Lexer *l;
    Token cur_token;
    Token peek_token;
    char **errors;
    int error_count;
    int error_capacity;
} Parser;

Parser *new_parser(Lexer *l);
Program *parse_program(Parser *p);
void free_parser(Parser *p);

#endif
