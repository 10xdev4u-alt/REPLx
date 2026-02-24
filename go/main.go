package main

import (
	"fmt"
	"io"
	"os"

	"github.com/chzyer/readline"
	"github.com/princetheprogrammerbtw/replngne/go/lexer"
	"github.com/princetheprogrammerbtw/replngne/go/token"
)

func main() {
	rl, err := readline.NewEx(&readline.Config{
		Prompt:          ">>> ",
		HistoryFile:     "/tmp/readline.tmp",
		AutoComplete:    nil,
		InterruptPrompt: "^C",
		EOFPrompt:       "exit",
	})
	if err != nil {
		panic(err)
	}
	defer rl.Close()

	fmt.Println("REPLngne (Go Edition) - v0.1.0")
	fmt.Println("Type 'exit' or Ctrl+D to quit")

	for {
		line, err := rl.Readline()
		if err == readline.ErrInterrupt {
			if len(line) == 0 {
				break
			} else {
				continue
			}
		} else if err == io.EOF {
			break
		}

		l := lexer.New(line)
		for {
			tok := l.NextToken()
			if tok.Type == token.EOF {
				break
			}
			fmt.Printf("%+v\n", tok)
		}
	}
	os.Exit(0)
}
