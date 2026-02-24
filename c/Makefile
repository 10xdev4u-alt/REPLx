CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g

all: replngne

replngne: main.o lexer.o ast.o parser.o
	$(CC) $(CFLAGS) -o replngne main.o lexer.o ast.o parser.o

main.o: main.c lexer.h token.h ast.h parser.h
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c lexer.h token.h
	$(CC) $(CFLAGS) -c lexer.c

ast.o: ast.c ast.h token.h
	$(CC) $(CFLAGS) -c ast.c

parser.o: parser.c parser.h lexer.h ast.h token.h
	$(CC) $(CFLAGS) -c parser.c

clean:
	rm -f replngne *.o

.PHONY: all clean
