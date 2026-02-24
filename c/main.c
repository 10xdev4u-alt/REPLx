#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "token.h"

#define MAX_INPUT 1024

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_ILLEGAL: return "ILLEGAL";
        case TOKEN_EOF: return "EOF";
        case TOKEN_IDENT: return "IDENT";
        case TOKEN_INT: return "INT";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_BANG: return "BANG";
        case TOKEN_ASTERISK: return "ASTERISK";
        case TOKEN_SLASH: return "SLASH";
        case TOKEN_LT: return "LT";
        case TOKEN_GT: return "GT";
        case TOKEN_EQ: return "EQ";
        case TOKEN_NOT_EQ: return "NOT_EQ";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_FUNCTION: return "FUNCTION";
        case TOKEN_LET: return "LET";
        case TOKEN_TRUE: return "TRUE";
        case TOKEN_FALSE: return "FALSE";
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_RETURN: return "RETURN";
        default: return "UNKNOWN";
    }
}

int main() {
    char buffer[MAX_INPUT];
    
    printf("REPLngne (C Edition) - v0.1.0\n");
    printf("Type 'exit' to quit.\n");

    while (1) {
        printf(">>> ");
        if (fgets(buffer, MAX_INPUT, stdin) == NULL) {
            break;
        }

        // Remove newline
        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        Lexer *l = new_lexer(buffer);
        
        Token tok;
        do {
            tok = next_token(l);
            printf("{Type: %s, Literal: %s}\n", token_type_to_string(tok.type), tok.literal);
            if (tok.type != TOKEN_EOF) {
                free(tok.literal);
            }
        } while (tok.type != TOKEN_EOF);

        free_lexer(l);
    }

    return 0;
}
