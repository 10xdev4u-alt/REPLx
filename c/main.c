#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "token.h"
#include "ast.h"
#include "parser.h"

#define MAX_INPUT 1024

int main() {
    char buffer[MAX_INPUT];
    
    printf("REPLngne (C Edition) - v0.2.0 (Parser Active)\n");
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
        Parser *p = new_parser(l);
        Program *program = parse_program(p);

        if (p->error_count > 0) {
            printf("Parser errors:\n");
            for (int i = 0; i < p->error_count; i++) {
                printf("\t%s\n", p->errors[i]);
            }
        } else {
            char *prog_str = program->base.string((ASTNode*)program);
            printf("%s\n", prog_str);
            free(prog_str);
        }

        program->base.free((ASTNode*)program);
        free_parser(p);
        free_lexer(l);
    }

    return 0;
}
