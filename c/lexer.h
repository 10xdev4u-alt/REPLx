#ifndef LEXER_H
#define LEXER_H

#include "token.h"

typedef struct {
    char *input;
    int position;      // current position in input (points to current char)
    int read_position; // current reading position in input (after current char)
    char ch;           // current char under examination
} Lexer;

Lexer *new_lexer(const char *input);
Token next_token(Lexer *l);
void free_lexer(Lexer *l);

#endif
