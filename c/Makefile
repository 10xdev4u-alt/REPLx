CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

all: replngne

replngne: main.o lexer.o
	$(CC) $(CFLAGS) -o replngne main.o lexer.o

main.o: main.c lexer.h token.h
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c lexer.h token.h
	$(CC) $(CFLAGS) -c lexer.c

clean:
	rm -f replngne *.o

.PHONY: all clean
