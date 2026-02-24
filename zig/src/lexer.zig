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
        while (self.ch == ' ' or self.ch == '\t' or self.ch == '\n' or self.ch == '\r') {
            self.readChar();
        }
    }

    pub fn nextToken(self: *Lexer) Token {
        self.skipWhitespace();

        var tok: Token = undefined;

        switch (self.ch) {
            '=' => {
                if (self.peekChar() == '=') {
                    const start = self.position;
                    self.readChar(); // consume first =
                    // self.ch is now second =. pos points to it.
                    // literal is "==".
                    // Wait, readChar advances pos.
                    // Initial: pos points to first =.
                    // readChar: pos points to second =.
                    // But we want slice covering both.
                    // Actually, let's keep it simple.
                    // current pos is first =.
                    self.readChar(); // consume second =
                    tok = Token{ .type = .EQ, .literal = self.input[start .. self.position + 1] };
                    // Wait, logic is tricky with indices.
                    // Let's rely on explicit chars for known operators.
                    tok = Token{ .type = .EQ, .literal = "==" };
                } else {
                    tok = Token{ .type = .ASSIGN, .literal = "=" };
                }
            },
            '+' => tok = Token{ .type = .PLUS, .literal = "+" },
            '-' => tok = Token{ .type = .MINUS, .literal = "-" },
            '!' => {
                if (self.peekChar() == '=') {
                    self.readChar();
                    tok = Token{ .type = .NOT_EQ, .literal = "!=" };
                } else {
                    tok = Token{ .type = .BANG, .literal = "!" };
                }
            },
            '/' => tok = Token{ .type = .SLASH, .literal = "/" },
            '*' => tok = Token{ .type = .ASTERISK, .literal = "*" },
            '<' => tok = Token{ .type = .LT, .literal = "<" },
            '>' => tok = Token{ .type = .GT, .literal = ">" },
            ';' => tok = Token{ .type = .SEMICOLON, .literal = ";" },
            ',' => tok = Token{ .type = .COMMA, .literal = "," },
            '(' => tok = Token{ .type = .LPAREN, .literal = "(" },
            ')' => tok = Token{ .type = .RPAREN, .literal = ")" },
            '{' => tok = Token{ .type = .LBRACE, .literal = "{" },
            '}' => tok = Token{ .type = .RBRACE, .literal = "}" },
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
                    tok = Token{ .type = .ILLEGAL, .literal = "" };
                }
            },
        }

        self.readChar();
        return tok;
    }

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
    return std.ascii.isAlphabetic(ch) or ch == '_';
}

fn isDigit(ch: u8) bool {
    return std.ascii.isDigit(ch);
}
