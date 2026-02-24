package main

import (
	"fmt"
	"io"
	"os"

	"github.com/chzyer/readline"
	"github.com/princetheprogrammerbtw/replngne/go/lexer"
	"github.com/princetheprogrammerbtw/replngne/go/parser"
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

	fmt.Println("REPLngne (Go Edition) - v0.2.0 (Parser Active)")
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
		p := parser.New(l)
		program := p.ParseProgram()

		if len(p.Errors()) != 0 {
			printParserErrors(os.Stdout, p.Errors())
			continue
		}

		if program != nil {
			io.WriteString(os.Stdout, program.String()+"\n")
		}
	}
	os.Exit(0)
}

func printParserErrors(out io.Writer, errors []string) {
	io.WriteString(out, "Parser errors:\n")
	for _, msg := range errors {
		io.WriteString(out, "\t"+msg+"\n")
	}
}
