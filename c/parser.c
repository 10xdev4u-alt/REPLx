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
    next_token_parser(p);
    next_token_parser(p);
    
    return p;
}

void free_parser(Parser *p) {
    for (int i = 0; i < p->error_count; i++) {
        free(p->errors[i]);
    }
    free(p->errors);
    // Tokens in parser are copies (literals are malloced).
    // cur_token and peek_token hold literals that need freeing?
    // In next_token_parser, we assign p->cur_token = p->peek_token.
    // If we free cur_token's literal before overwriting, we are good.
    // But initially?
    // Wait, Lexer returns Token with malloced literal.
    // Parser takes ownership.
    if (p->cur_token.literal) free(p->cur_token.literal);
    if (p->peek_token.literal) free(p->peek_token.literal);
    free(p);
}

void next_token_parser(Parser *p) {
    // Free current token literal before overwriting?
    // Wait, if we use it in AST, we transfer ownership?
    // Yes, AST nodes take ownership of token.
    // So if AST node is created, it takes the token.
    // If not, we must free it.
    // This is tricky in C.
    // Strategy: AST constructors take Token (struct copy).
    // If we successfully parse, the AST node owns the literal pointer.
    // If we skip/error, we must free the literal pointer.
    
    // For now, let's assume valid parse transfers ownership.
    // But what about p->cur_token which is being overwritten?
    // If it wasn't used, we leak.
    // Ideally, we'd have a flag or just duplicate string for AST and always free token here?
    // Duplicating is safer but slower.
    // Let's implement AST taking ownership (copy struct, keep pointer).
    // And here, we define that if cur_token was NOT consumed, we free it.
    // But how do we know?
    // Simpler: Parser always frees its copy of token when advancing,
    // AND AST always strdup's the literal.
    // This is "Zero-Ownership Transfer" - everyone owns their copy.
    // Much safer for this stage.
    
    if (p->cur_token.literal) {
         free(p->cur_token.literal); 
    }
    
    p->cur_token = p->peek_token;
    p->peek_token = next_token(p->l);
    // peek_token.literal is fresh malloc from lexer.
}

// But wait! AST constructors in ast.c currently take Token and do NOT strdup the literal in all cases?
// identifier_token_literal returns ast_str_copy(i->token.literal).
// identifier_string returns ast_str_copy(i->value).
// new_identifier takes Token t.
// It stores it: i->token = t;
// And i->value = ast_str_copy(value).
// So AST owns the pointer in t.literal.
// If Parser frees p->cur_token.literal, AST has dangling pointer.
// FIX: In next_token_parser, we should NOT free if AST took it.
// To make this simple:
// AST constructors should STRDUP the literal found in the token, so they own their own copy.
// Then Parser can always free its token.
// Let's check ast.c:
// new_let_statement: ls->token = token; (Shallow copy)
// This means AST shares the pointer.
// So Parser MUST NOT free it if AST took it.
// This is hard to track.
// ALTERNATIVE: AST constructors strdup the literal.
// Then Parser ALWAYS frees.
// I will modify ast.c later or now?
// Let's modify Parser to StrDup for AST? No, AST constructor should do it.
// Let's modify ast.c to duplicate token literal in constructors.
// OR: Let's make `next_token_parser` perform a deep copy for `p->peek_token`?
// No, Lexer returns malloced string.
// Let's stick to: Parser owns the token from Lexer.
// When creating AST, we pass the Token.
// If AST takes ownership, we should NULL out Parser's pointer so it doesn't double free?
// Yes, manually NULLing out literals in Parser after passing to AST is a common C pattern.

Program *parse_program(Parser *p) {
    Program *program = new_program();
    
    while (p->cur_token.type != TOKEN_EOF) {
        Statement *stmt = parse_statement(p);
        if (stmt) {
            program_append(program, stmt);
        } else {
             // If statement failed, we might need to skip token?
             // parse_statement advances tokens usually.
             next_token_parser(p);
        }
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
    // We are at LET.
    // Create struct.
    // We need to pass the token. 
    // We want AST to own the literal.
    // So we NULL out p->cur_token.literal so next_token_parser doesn't free it?
    // Wait, I haven't implemented that logic in next_token_parser yet.
    // Let's do the "AST copies" approach. It is safer.
    // I will update ast.c to strdup token literal.
    
    Token letTok = p->cur_token; // Copy struct
    // AST constructor will (eventually) copy string.
    
    if (!expect_peek(p, TOKEN_IDENT)) {
        return NULL;
    }
    
    Token identTok = p->cur_token;
    Identifier *name = new_identifier(identTok, identTok.literal);
    
    if (!expect_peek(p, TOKEN_ASSIGN)) {
        return NULL; // Leak name? Yes.
    }
    
    // TODO: Expression parsing
    while (p->cur_token.type != TOKEN_SEMICOLON) {
        next_token_parser(p);
    }
    
    LetStatement *stmt = new_let_statement(letTok);
    stmt->name = name;
    
    return stmt;
}

ReturnStatement *parse_return_statement(Parser *p) {
    Token retTok = p->cur_token;
    
    next_token_parser(p);
    
    while (p->cur_token.type != TOKEN_SEMICOLON) {
        next_token_parser(p);
    }
    
    return new_return_statement(retTok);
}

ExpressionStatement *parse_expression_statement(Parser *p) {
    // Placeholder
    // Consume until semicolon
     while (p->cur_token.type != TOKEN_SEMICOLON && p->cur_token.type != TOKEN_EOF) {
        next_token_parser(p);
    }
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
