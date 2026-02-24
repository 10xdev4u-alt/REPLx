#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Keywords
    Let,
    Fn,
    If,
    Else,
    Return,
    True,
    False,

    // Identifiers + Literals
    Ident(String),
    Int(i64),
    String(String),

    // Operators
    Assign,
    Plus,
    Minus,
    Bang,
    Asterisk,
    Slash,
    Equal,
    NotEqual,
    LessThan,
    GreaterThan,
    
    // Delimiters
    Comma,
    Semicolon,
    LParen,
    RParen,
    LBrace,
    RBrace,

    // End of File
    Illegal,
    EOF,
}

impl Token {
    pub fn lookup_ident(ident: &str) -> Token {
        match ident {
            "fn" => Token::Fn,
            "let" => Token::Let,
            "true" => Token::True,
            "false" => Token::False,
            "if" => Token::If,
            "else" => Token::Else,
            "return" => Token::Return,
            _ => Token::Ident(ident.to_string()),
        }
    }
}
