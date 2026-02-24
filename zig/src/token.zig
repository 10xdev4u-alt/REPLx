const std = @import("std");

pub const TokenType = enum {
    ILLEGAL,
    EOF,

    // Identifiers + Literals
    IDENT,
    INT,
    STRING,

    // Operators
    ASSIGN,
    PLUS,
    MINUS,
    BANG,
    ASTERISK,
    SLASH,

    LT,
    GT,

    EQ,
    NOT_EQ,

    // Delimiters
    COMMA,
    SEMICOLON,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,

    // Keywords
    FUNCTION,
    LET,
    TRUE,
    FALSE,
    IF,
    ELSE,
    RETURN,
};

pub const Token = struct {
    type: TokenType,
    literal: []const u8,

    pub fn lookupIdent(ident: []const u8) TokenType {
        if (std.mem.eql(u8, ident, "fn")) return .FUNCTION;
        if (std.mem.eql(u8, ident, "let")) return .LET;
        if (std.mem.eql(u8, ident, "true")) return .TRUE;
        if (std.mem.eql(u8, ident, "false")) return .FALSE;
        if (std.mem.eql(u8, ident, "if")) return .IF;
        if (std.mem.eql(u8, ident, "else")) return .ELSE;
        if (std.mem.eql(u8, ident, "return")) return .RETURN;
        return .IDENT;
    }
};
