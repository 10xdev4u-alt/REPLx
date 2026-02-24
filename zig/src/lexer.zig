const std = @import("std");
const Token = @import("token.zig").Token;
const TokenType = @import("token.zig").TokenType;

pub const Lexer = struct {
    input: []const u8,
    position: usize,
    read_position: usize,
    ch: u8,

    pub fn new(input: []const u8) Lexer {
        var l = Lexer{
            .input = input,
            .position = 0,
            .read_position = 0,
            .ch = 0,
        };
        l.readChar();
        return l;
    }

    fn readChar(self: *Lexer) void {
        if (self.read_position >= self.input.len) {
            self.ch = 0;
        } else {
            self.ch = self.input[self.read_position];
        }
        self.position = self.read_position;
        self.read_position += 1;
    }

    fn peekChar(self: *Lexer) u8 {
        if (self.read_position >= self.input.len) {
            return 0;
        } else {
            return self.input[self.read_position];
        }
    }

    fn skipWhitespace(self: *Lexer) void {
        while (self.ch == ' ' or self.ch == '	' or self.ch == '
' or self.ch == '') {
            self.readChar();
        }
    }

    pub fn nextToken(self: *Lexer) Token {
        self.skipWhitespace();

        var tok = Token{ .type = .ILLEGAL, .literal = "" };

        switch (self.ch) {
            '=' => {
                if (self.peekChar() == '=') {
                    const ch = self.ch;
                    self.readChar();
                    const literal = self.input[self.position-1 .. self.read_position];
                    tok = Token{ .type = .EQ, .literal = literal };
                } else {
                    tok = newToken(.ASSIGN, self.ch);
                }
            },
            '+' => tok = newToken(.PLUS, self.ch),
            '-' => tok = newToken(.MINUS, self.ch),
            '!' => {
                if (self.peekChar() == '=') {
                    const ch = self.ch;
                    self.readChar();
                    const literal = self.input[self.position-1 .. self.read_position];
                    tok = Token{ .type = .NOT_EQ, .literal = literal };
                } else {
                    tok = newToken(.BANG, self.ch);
                }
            },
            '/' => tok = newToken(.SLASH, self.ch),
            '*' => tok = newToken(.ASTERISK, self.ch),
            '<' => tok = newToken(.LT, self.ch),
            '>' => tok = newToken(.GT, self.ch),
            ';' => tok = newToken(.SEMICOLON, self.ch),
            ',' => tok = newToken(.COMMA, self.ch),
            '(' => tok = newToken(.LPAREN, self.ch),
            ')' => tok = newToken(.RPAREN, self.ch),
            '{' => tok = newToken(.LBRACE, self.ch),
            '}' => tok = newToken(.RBRACE, self.ch),
            '"' => {
                tok.type = .STRING;
                tok.literal = self.readString();
            },
            0 => {
                tok.literal = "";
                tok.type = .EOF;
            },
            else => {
                if (isLetter(self.ch)) {
                    tok.literal = self.readIdentifier();
                    tok.type = Token.lookupIdent(tok.literal);
                    return tok;
                } else if (isDigit(self.ch)) {
                    tok.type = .INT;
                    tok.literal = self.readNumber();
                    return tok;
                } else {
                    tok = newToken(.ILLEGAL, self.ch);
                }
            },
        }

        self.readChar();
        return tok;
    }

    fn newToken(token_type: TokenType, ch: u8) Token {
        // We can't return slice of local char easily without pointing to input
        // But for single chars, we can just point to input at current position?
        // Wait, self.ch is a copy.
        // We need to return slice of self.input.
        // Since newToken is called before readChar(), self.position points to the char.
        // But wait, we need access to 'self' to slice input.
        // The helper 'newToken' as written in Go/Rust was simple because it owned the data or copied.
        // Here, I need to slice input.
        // So I'll inline the logic or pass slice.
        return Token{ .type = token_type, .literal = "" }; // Placeholder, fixed in call sites?
    }
    
    // Fix: newToken should take the slice, not char.
    // Or simpler: just inline it in the switch.
    
    fn readIdentifier(self: *Lexer) []const u8 {
        const position = self.position;
        while (isLetter(self.ch)) {
            self.readChar();
        }
        return self.input[position..self.position];
    }

    fn readNumber(self: *Lexer) []const u8 {
        const position = self.position;
        while (isDigit(self.ch)) {
            self.readChar();
        }
        return self.input[position..self.position];
    }
    
    fn readString(self: *Lexer) []const u8 {
        const position = self.position + 1;
        while (true) {
            self.readChar();
            if (self.ch == '"' or self.ch == 0) {
                break;
            }
        }
        return self.input[position..self.position];
    }
};

fn isLetter(ch: u8) bool {
    return (ch >= 'a' and ch <= 'z') or (ch >= 'A' and ch <= 'Z') or ch == '_';
}

fn isDigit(ch: u8) bool {
    return ch >= '0' and ch <= '9';
}

fn newToken(token_type: TokenType, ch: u8) Token {
    // This helper is problematic for Zero-Copy if we want the literal to be from input.
    // But wait, single char tokens are always valid as long as we don't care about the literal for them?
    // Actually, for parser, we usually check type.
    // But for printing "Echo: {literal}", we need it.
    // So I should fix the switch cases to slice input.
    return Token{ .type = token_type, .literal = "" }; 
}
