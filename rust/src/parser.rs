use crate::lexer::Lexer;
use crate::token::Token;
use crate::ast::{Program, Statement, LetStatement, ReturnStatement, Identifier, ExpressionStatement, Expression, IntegerLiteral, PrefixExpression, InfixExpression};

#[derive(PartialOrd, PartialEq, Clone, Copy)]
enum Precedence {
    Lowest,
    Equals,      // ==
    LessGreater, // > or <
    Sum,         // +
    Product,     // *
    Prefix,      // -X or !X
    Call,        // myFunction(X)
}

fn get_precedence(tok: &Token) -> Precedence {
    match tok {
        Token::Equal | Token::NotEqual => Precedence::Equals,
        Token::LessThan | Token::GreaterThan => Precedence::LessGreater,
        Token::Plus | Token::Minus => Precedence::Sum,
        Token::Slash | Token::Asterisk => Precedence::Product,
        _ => Precedence::Lowest,
    }
}

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
        if !self.expect_peek(Token::Ident("".to_string())) { return None; }

        let name = Identifier {
            token: self.cur_token.clone(),
            value: match &self.cur_token {
                Token::Ident(val) => val.clone(),
                _ => return None,
            },
        };

        if !self.expect_peek(Token::Assign) { return None; }
        self.next_token();

        let value = self.parse_expression(Precedence::Lowest);

        if self.peek_token_is(Token::Semicolon) {
            self.next_token();
        }

        Some(Box::new(LetStatement { token, name, value }))
    }

    fn parse_return_statement(&mut self) -> Option<Box<dyn Statement>> {
        let token = self.cur_token.clone();
        self.next_token();

        let return_value = self.parse_expression(Precedence::Lowest);

        if self.peek_token_is(Token::Semicolon) {
            self.next_token();
        }

        Some(Box::new(ReturnStatement { token, return_value }))
    }

    fn parse_expression_statement(&mut self) -> Option<Box<dyn Statement>> {
        let token = self.cur_token.clone();
        let expression = self.parse_expression(Precedence::Lowest);

        if self.peek_token_is(Token::Semicolon) {
            self.next_token();
        }

        Some(Box::new(ExpressionStatement { token, expression }))
    }

    fn parse_expression(&mut self, precedence: Precedence) -> Option<Box<dyn Expression>> {
        let mut left_exp = self.parse_prefix()?;

        while !self.peek_token_is(Token::Semicolon) && precedence < self.peek_precedence() {
            self.next_token();
            left_exp = self.parse_infix(left_exp)?;
        }

        Some(left_exp)
    }

    fn parse_prefix(&mut self) -> Option<Box<dyn Expression>> {
        match &self.cur_token {
            Token::Ident(val) => Some(Box::new(Identifier {
                token: self.cur_token.clone(),
                value: val.clone(),
            })),
            Token::Int(val) => Some(Box::new(IntegerLiteral {
                token: self.cur_token.clone(),
                value: *val,
            })),
            Token::Bang | Token::Minus => {
                let token = self.cur_token.clone();
                let operator = match &token {
                    Token::Bang => "!".to_string(),
                    Token::Minus => "-".to_string(),
                    _ => unreachable!(),
                };
                self.next_token();
                let right = self.parse_expression(Precedence::Prefix)?;
                Some(Box::new(PrefixExpression { token, operator, right }))
            },
            Token::LParen => {
                self.next_token();
                let exp = self.parse_expression(Precedence::Lowest);
                if !self.expect_peek(Token::RParen) {
                    return None;
                }
                exp
            },
            _ => {
                self.errors.push(format!("no prefix parse function for {:?} found", self.cur_token));
                None
            }
        }
    }

    fn parse_infix(&mut self, left: Box<dyn Expression>) -> Option<Box<dyn Expression>> {
        let token = self.cur_token.clone();
        let operator = match &token {
            Token::Plus => "+".to_string(),
            Token::Minus => "-".to_string(),
            Token::Asterisk => "*".to_string(),
            Token::Slash => "/".to_string(),
            Token::Equal => "==".to_string(),
            Token::NotEqual => "!=".to_string(),
            Token::LessThan => "<".to_string(),
            Token::GreaterThan => ">".to_string(),
            _ => return Some(left),
        };

        let precedence = self.cur_precedence();
        self.next_token();
        let right = self.parse_expression(precedence)?;

        Some(Box::new(InfixExpression { token, left, operator, right }))
    }

    fn cur_precedence(&self) -> Precedence {
        get_precedence(&self.cur_token)
    }

    fn peek_precedence(&self) -> Precedence {
        get_precedence(&self.peek_token)
    }

    fn cur_token_is(&self, t: Token) -> bool {
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
        let input = "let x = 5; let y = 10; let foobar = 838383;";
        let l = Lexer::new(input.to_string());
        let mut p = Parser::new(l);
        let program = p.parse_program().unwrap();
        assert_eq!(program.statements.len(), 3);
    }

    #[test]
    fn test_expression_parsing() {
        let input = "-a * b; !-5; 5 + 5; 5 > 5 == 3 < 5;";
        let l = Lexer::new(input.to_string());
        let mut p = Parser::new(l);
        let program = p.parse_program().unwrap();
        
        let expected = vec![
            "((-a) * b)",
            "(!(-5))",
            "(5 + 5)",
            "((5 > 5) == (3 < 5))"
        ];
        
        for (i, s) in expected.iter().enumerate() {
            assert_eq!(program.statements[i].string(), *s);
        }
    }
}
