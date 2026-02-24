use crate::token::Token;

pub trait Node {
    fn token_literal(&self) -> String;
    fn string(&self) -> String;
}

pub trait Statement: Node {
    fn statement_node(&self);
}

pub trait Expression: Node {
    fn expression_node(&self);
}

pub struct Program {
    pub statements: Vec<Box<dyn Statement>>,
}

impl Node for Program {
    fn token_literal(&self) -> String {
        if self.statements.len() > 0 {
            self.statements[0].token_literal()
        } else {
            "".to_string()
        }
    }

    fn string(&self) -> String {
        let mut out = String::new();
        for s in &self.statements {
            out.push_str(&s.string());
        }
        out
    }
}

// Let Statement
pub struct LetStatement {
    pub token: Token, // Token::Let
    pub name: Identifier,
    pub value: Option<Box<dyn Expression>>,
}

impl Node for LetStatement {
    fn token_literal(&self) -> String {
        // Mapping Token to String is tricky if Token holds ownership
        // For now, return "let"
        "let".to_string()
    }
    
    fn string(&self) -> String {
        let mut out = String::new();
        out.push_str(&self.token_literal());
        out.push_str(" ");
        out.push_str(&self.name.string());
        out.push_str(" = ");
        if let Some(val) = &self.value {
            out.push_str(&val.string());
        }
        out.push_str(";");
        out
    }
}

impl Statement for LetStatement {
    fn statement_node(&self) {}
}

// Identifier
pub struct Identifier {
    pub token: Token, // Token::Ident
    pub value: String,
}

impl Node for Identifier {
    fn token_literal(&self) -> String {
        self.value.clone()
    }

    fn string(&self) -> String {
        self.value.clone()
    }
}

impl Expression for Identifier {
    fn expression_node(&self) {}
}

// Return Statement
pub struct ReturnStatement {
    pub token: Token, // Token::Return
    pub return_value: Option<Box<dyn Expression>>,
}

impl Node for ReturnStatement {
    fn token_literal(&self) -> String {
        "return".to_string()
    }

    fn string(&self) -> String {
        let mut out = String::new();
        out.push_str("return ");
        if let Some(val) = &self.return_value {
            out.push_str(&val.string());
        }
        out.push_str(";");
        out
    }
}

impl Statement for ReturnStatement {
    fn statement_node(&self) {}
}

// Expression Statement (wrapper for expressions used as statements)
pub struct ExpressionStatement {
    pub token: Token, // First token of expression
    pub expression: Option<Box<dyn Expression>>,
}

impl Node for ExpressionStatement {
    fn token_literal(&self) -> String {
        "expression".to_string()
    }

    fn string(&self) -> String {
        if let Some(exp) = &self.expression {
            exp.string()
        } else {
            "".to_string()
        }
    }
}

impl Statement for ExpressionStatement {
    fn statement_node(&self) {}
}
