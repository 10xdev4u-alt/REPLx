package ast

import (
	"bytes"
	"github.com/princetheprogrammerbtw/replngne/go/token"
)

// The Node interface is the base for all AST nodes.
type Node interface {
	TokenLiteral() string
	String() string
}

// Statement is a node that represents a statement (does not produce a value).
type Statement interface {
	Node
	statementNode()
}

// Expression is a node that represents an expression (produces a value).
type Expression interface {
	Node
	expressionNode()
}

// Program is the root node of the AST.
type Program struct {
	Statements []Statement
}

func (p *Program) TokenLiteral() string {
	if len(p.Statements) > 0 {
		return p.Statements[0].TokenLiteral()
	} else {
		return ""
	}
}

func (p *Program) String() string {
	var out bytes.Buffer
	for _, s := range p.Statements {
		out.WriteString(s.String())
	}
	return out.String()
}
