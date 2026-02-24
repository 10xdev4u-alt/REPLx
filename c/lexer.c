#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

// Helper: Duplicate string (safe copy)
char *str_copy(const char *s) {
    if (!s) return NULL;
    char *d = malloc(strlen(s) + 1);
    if (d) strcpy(d, s);
    return d;
}

// Helper: Create single char string
char *char_to_str(char c) {
    char *s = malloc(2);
    if (s) {
        s[0] = c;
        s[1] = '\0';
    }
    return s;
}

Lexer *new_lexer(const char *input) {
    Lexer *l = malloc(sizeof(Lexer));
    if (!l) return NULL;
    
    l->input = str_copy(input);
    l->read_position = 0;
    l->position = 0;
    
    // Initial read
    if (l->input && l->read_position >= (int)strlen(l->input)) {
        l->ch = 0;
    } else if (l->input) {
        l->ch = l->input[l->read_position];
    } else {
        l->ch = 0;
    }
    l->position = l->read_position;
    l->read_position++;
    
    return l;
}

void free_lexer(Lexer *l) {
    if (l) {
        if (l->input) free(l->input);
        free(l);
    }
}

void free_token(Token t) {
    if (t.literal) {
        free(t.literal);
    }
}

void read_char(Lexer *l) {
    if (!l->input || l->read_position >= (int)strlen(l->input)) {
        l->ch = 0;
    } else {
        l->ch = l->input[l->read_position];
    }
    l->position = l->read_position;
    l->read_position++;
}

char peek_char(Lexer *l) {
    if (!l->input || l->read_position >= (int)strlen(l->input)) {
        return 0;
    } else {
        return l->input[l->read_position];
    }
}

void skip_whitespace(Lexer *l) {
    while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r') {
        read_char(l);
    }
}

int is_letter(char ch) {
    return isalpha(ch) || ch == '_';
}

int is_digit_char(char ch) {
    return isdigit(ch);
}

char *read_identifier(Lexer *l) {
    int start_pos = l->position;
    while (is_letter(l->ch)) {
        read_char(l);
    }
    
    int len = l->position - start_pos;
    char *ident = malloc(len + 1);
    if (ident) {
        memcpy(ident, l->input + start_pos, len);
        ident[len] = '\0';
    }
    return ident;
}

char *read_number(Lexer *l) {
    int start_pos = l->position;
    while (is_digit_char(l->ch)) {
        read_char(l);
    }
    
    int len = l->position - start_pos;
    char *num = malloc(len + 1);
    if (num) {
        memcpy(num, l->input + start_pos, len);
        num[len] = '\0';
    }
    return num;
}

TokenType lookup_ident(const char *ident) {
    if (!ident) return TOKEN_ILLEGAL;
    if (strcmp(ident, "fn") == 0) return TOKEN_FUNCTION;
    if (strcmp(ident, "let") == 0) return TOKEN_LET;
    if (strcmp(ident, "true") == 0) return TOKEN_TRUE;
    if (strcmp(ident, "false") == 0) return TOKEN_FALSE;
    if (strcmp(ident, "if") == 0) return TOKEN_IF;
    if (strcmp(ident, "else") == 0) return TOKEN_ELSE;
    if (strcmp(ident, "return") == 0) return TOKEN_RETURN;
    return TOKEN_IDENT;
}

Token next_token(Lexer *l) {
    Token tok;
    // Initialize default
    tok.type = TOKEN_ILLEGAL;
    tok.literal = NULL;

    skip_whitespace(l);

    switch (l->ch) {
        case '=':
            if (peek_char(l) == '=') {
                read_char(l);
                tok.type = TOKEN_EQ;
                tok.literal = str_copy("==");
            } else {
                tok.type = TOKEN_ASSIGN;
                tok.literal = char_to_str('=');
            }
            break;
        case '+':
            tok.type = TOKEN_PLUS;
            tok.literal = char_to_str('+');
            break;
        case '-':
            tok.type = TOKEN_MINUS;
            tok.literal = char_to_str('-');
            break;
        case '!':
            if (peek_char(l) == '=') {
                read_char(l);
                tok.type = TOKEN_NOT_EQ;
                tok.literal = str_copy("!=");
            } else {
                tok.type = TOKEN_BANG;
                tok.literal = char_to_str('!');
            }
            break;
        case '/':
            tok.type = TOKEN_SLASH;
            tok.literal = char_to_str('/');
            break;
        case '*':
            tok.type = TOKEN_ASTERISK;
            tok.literal = char_to_str('*');
            break;
        case '<':
            tok.type = TOKEN_LT;
            tok.literal = char_to_str('<');
            break;
        case '>':
            tok.type = TOKEN_GT;
            tok.literal = char_to_str('>');
            break;
        case ';':
            tok.type = TOKEN_SEMICOLON;
            tok.literal = char_to_str(';');
            break;
        case ',':
            tok.type = TOKEN_COMMA;
            tok.literal = char_to_str(',');
            break;
        case '(':
            tok.type = TOKEN_LPAREN;
            tok.literal = char_to_str('(');
            break;
        case ')':
            tok.type = TOKEN_RPAREN;
            tok.literal = char_to_str(')');
            break;
        case '{':
            tok.type = TOKEN_LBRACE;
            tok.literal = char_to_str('{');
            break;
        case '}':
            tok.type = TOKEN_RBRACE;
            tok.literal = char_to_str('}');
            break;
        case 0:
            tok.type = TOKEN_EOF;
            tok.literal = str_copy("");
            break;
        default:
            if (is_letter(l->ch)) {
                tok.literal = read_identifier(l);
                tok.type = lookup_ident(tok.literal);
                return tok; // Early return
            } else if (is_digit_char(l->ch)) {
                tok.type = TOKEN_INT;
                tok.literal = read_number(l);
                return tok; // Early return
            } else {
                tok.type = TOKEN_ILLEGAL;
                tok.literal = char_to_str(l->ch);
            }
    }

    read_char(l);
    return tok;
}
