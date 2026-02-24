use crate::lexer::Lexer;
use crate::token::Token;
use crate::ast::{Program, Statement, LetStatement, ReturnStatement, Identifier, ExpressionStatement, Expression};

pub struct Parser {
    l: Lexer,
    cur_token: Token,
    peek_token: Token,
    pub errors: Vec<String>,
}

impl Parser {
    pub fn new(l: Lexer) -> Parser {
        let mut p = Parser {
            l,
            cur_token: Token::EOF,
            peek_token: Token::EOF,
            errors: vec![],
        };
        // Read two tokens to setup cur and peek
        p.next_token();
        p.next_token();
        p
    }

    fn next_token(&mut self) {
        self.cur_token = self.peek_token.clone();
        self.peek_token = self.l.next_token();
    }

    pub fn parse_program(&mut self) -> Option<Program> {
        let mut program = Program {
            statements: vec![],
        };

        while self.cur_token != Token::EOF {
            if let Some(stmt) = self.parse_statement() {
                program.statements.push(stmt);
            }
            self.next_token();
        }

        Some(program)
    }

    fn parse_statement(&mut self) -> Option<Box<dyn Statement>> {
        match self.cur_token {
            Token::Let => self.parse_let_statement(),
            Token::Return => self.parse_return_statement(),
            _ => self.parse_expression_statement(),
        }
    }

    fn parse_let_statement(&mut self) -> Option<Box<dyn Statement>> {
        let token = self.cur_token.clone();

        if !self.expect_peek(Token::Ident("".to_string())) {
            return None;
        }

        let name = Identifier {
            token: self.cur_token.clone(),
            value: match &self.cur_token {
                Token::Ident(val) => val.clone(),
                _ => return None,
            },
        };

        if !self.expect_peek(Token::Assign) {
            return None;
        }

        // TODO: We're skipping expression parsing for now to keep it compilable
        // We'll advance until semicolon
        while !self.cur_token_is(Token::Semicolon) {
            self.next_token();
        }

        Some(Box::new(LetStatement {
            token,
            name,
            value: None, // Placeholder
        }))
    }

    fn parse_return_statement(&mut self) -> Option<Box<dyn Statement>> {
        let token = self.cur_token.clone();

        self.next_token();

        // TODO: Skip expression
        while !self.cur_token_is(Token::Semicolon) {
            self.next_token();
        }

        Some(Box::new(ReturnStatement {
            token,
            return_value: None, // Placeholder
        }))
    }
    
    fn parse_expression_statement(&mut self) -> Option<Box<dyn Statement>> {
        // Placeholder
        None
    }

    fn cur_token_is(&self, t: Token) -> bool {
        // We need discriminant checking, but Token holds data.
        // We can use std::mem::discriminant or match
        // For now, simple match or derive PartialEq (which we did)
        // But PartialEq checks data too.
        // Token::Ident("a") != Token::Ident("b")
        // So checking type only is tricky with PartialEq
        match (&self.cur_token, &t) {
            (Token::Ident(_), Token::Ident(_)) => true,
            (Token::Int(_), Token::Int(_)) => true,
            (Token::String(_), Token::String(_)) => true,
            (t1, t2) => t1 == t2,
        }
    }

    fn peek_token_is(&self, t: Token) -> bool {
        match (&self.peek_token, &t) {
            (Token::Ident(_), Token::Ident(_)) => true,
            (Token::Int(_), Token::Int(_)) => true,
            (Token::String(_), Token::String(_)) => true,
            (t1, t2) => t1 == t2,
        }
    }

    fn expect_peek(&mut self, t: Token) -> bool {
        if self.peek_token_is(t.clone()) {
            self.next_token();
            true
        } else {
            self.peek_error(t);
            false
        }
    }

    fn peek_error(&mut self, t: Token) {
        let msg = format!("expected next token to be {:?}, got {:?} instead", t, self.peek_token);
        self.errors.push(msg);
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::Lexer;

    #[test]
    fn test_let_statements() {
        let input = "
let x = 5;
let y = 10;
let foobar = 838383;
";
        let l = Lexer::new(input.to_string());
        let mut p = Parser::new(l);

        let program = p.parse_program().unwrap();
        if p.errors.len() > 0 {
            panic!("parser has {} errors: {:?}", p.errors.len(), p.errors);
        }

        if program.statements.len() != 3 {
            panic!("program.statements does not contain 3 statements. got={}",
                program.statements.len());
        }

        let tests = vec!["x", "y", "foobar"];

        for (i, expected_ident) in tests.iter().enumerate() {
            let stmt = &program.statements[i];
            test_let_statement(stmt, expected_ident);
        }
    }

    fn test_let_statement(s: &Box<dyn Statement>, name: &str) {
        if s.token_literal() != "let" {
            panic!("s.token_literal not 'let'. got={}", s.token_literal());
        }
        
        // We can't easily downcast Box<dyn Statement> to LetStatement without Any trait
        // But we can check string output or just trust it parsed if no errors
        // Ideally we'd use Any for downcasting
        // For this simple test, we'll verify the string representation matches roughly
        // "let x = ;" (since expression is skipped)
        let expected = format!("let {} = ;", name);
        if s.string() != expected {
             panic!("stmt.string() not '{}'. got={}", expected, s.string());
        }
    }
}
