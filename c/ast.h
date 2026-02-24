#ifndef AST_H
#define AST_H

#include "token.h"

typedef enum {
    NODE_PROGRAM,
    NODE_LET,
    NODE_RETURN,
    NODE_EXPRESSION_STMT,
    NODE_IDENTIFIER
} NodeType;

typedef struct ASTNode {
    NodeType type;
    char *(*token_literal)(struct ASTNode *);
    char *(*string)(struct ASTNode *);
    void (*free)(struct ASTNode *);
} ASTNode;

// Identifier
typedef struct {
    ASTNode base;
    Token token;
    char *value;
} Identifier;

// Expression (Abstract)
typedef ASTNode Expression;

// LetStatement
typedef struct {
    ASTNode base;
    Token token;
    Identifier *name;
    Expression *value;
} LetStatement;

// ReturnStatement
typedef struct {
    ASTNode base;
    Token token;
    Expression *return_value;
} ReturnStatement;

// ExpressionStatement
typedef struct {
    ASTNode base;
    Token token;
    Expression *expression;
} ExpressionStatement;

// Statement (Abstract wrapper)
typedef ASTNode Statement;

// Program
typedef struct {
    ASTNode base;
    Statement **statements;
    int statement_count;
    int capacity;
} Program;

// Constructors
Program *new_program();
LetStatement *new_let_statement(Token token);
ReturnStatement *new_return_statement(Token token);
Identifier *new_identifier(Token token, char *value);
ExpressionStatement *new_expression_statement(Token token);

void program_append(Program *p, Statement *stmt);

#endif
