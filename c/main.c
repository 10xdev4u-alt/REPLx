#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 1024

int main() {
    char buffer[MAX_INPUT];
    
    printf("REPLngne (C Edition)
");
    printf("Type 'exit' to quit.
");

    while (1) {
        printf(">>> ");
        if (fgets(buffer, MAX_INPUT, stdin) == NULL) {
            break;
        }

        // Remove newline
        buffer[strcspn(buffer, "
")] = 0;

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        printf("Echo: %s
", buffer);
    }

    return 0;
}
