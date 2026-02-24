#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    TOKEN_ILLEGAL,
    TOKEN_EOF,
    
    // Identifiers + Literals
    TOKEN_IDENT,
    TOKEN_INT,
    TOKEN_STRING,

    // Operators
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_BANG,
    TOKEN_ASTERISK,
    TOKEN_SLASH,
    
    TOKEN_LT,
    TOKEN_GT,
    
    TOKEN_EQ,
    TOKEN_NOT_EQ,

    // Delimiters
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,

    // Keywords
    TOKEN_FUNCTION,
    TOKEN_LET,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_RETURN
} TokenType;

typedef struct {
    TokenType type;
    char *literal;
} Token;

void free_token(Token t);

#endif
