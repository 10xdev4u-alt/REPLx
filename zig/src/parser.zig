const std = @import("std");
const Lexer = @import("lexer.zig").Lexer;
const Token = @import("token.zig").Token;
const TokenType = @import("token.zig").TokenType;
const ast = @import("ast.zig");

pub const Parser = struct {
    l: Lexer,
    cur_token: Token,
    peek_token: Token,
    errors: std.ArrayList([]const u8),
    allocator: std.mem.Allocator,

    pub fn init(l: Lexer, allocator: std.mem.Allocator) Parser {
        var p = Parser{
            .l = l,
            .cur_token = undefined,
            .peek_token = undefined,
            .errors = std.ArrayList([]const u8).init(allocator),
            .allocator = allocator,
        };
        // Read two tokens
        p.nextToken();
        p.nextToken();
        return p;
    }

    pub fn deinit(self: *Parser) void {
        for (self.errors.items) |msg| {
            self.allocator.free(msg);
        }
        self.errors.deinit();
    }

    fn nextToken(self: *Parser) void {
        self.cur_token = self.peek_token;
        self.peek_token = self.l.nextToken();
    }

    pub fn parseProgram(self: *Parser) !ast.Program {
        var program = ast.Program.init(self.allocator);
        
        while (self.cur_token.type != .EOF) {
            const stmt = try self.parseStatement();
            if (stmt) |s| {
                try program.statements.append(s);
            }
            self.nextToken();
        }
        return program;
    }

    fn parseStatement(self: *Parser) !?ast.Statement {
        switch (self.cur_token.type) {
            .LET => return try self.parseLetStatement(),
            .RETURN => return try self.parseReturnStatement(),
            else => return try self.parseExpressionStatement(),
        }
    }

    fn parseLetStatement(self: *Parser) !?ast.Statement {
        var stmt = try self.allocator.create(ast.LetStatement);
        stmt.token = self.cur_token;

        if (!self.expectPeek(.IDENT)) {
            return null;
        }

        var ident = try self.allocator.create(ast.Identifier);
        ident.token = self.cur_token;
        ident.value = self.cur_token.literal;
        stmt.name = ident;

        if (!self.expectPeek(.ASSIGN)) {
            return null;
        }

        while (!self.curTokenIs(.SEMICOLON) and !self.curTokenIs(.EOF)) {
            self.nextToken();
        }
        
        stmt.value = null;

        return ast.Statement{ .Let = stmt };
    }

    fn parseReturnStatement(self: *Parser) !?ast.Statement {
        var stmt = try self.allocator.create(ast.ReturnStatement);
        stmt.token = self.cur_token;

        self.nextToken();

        while (!self.curTokenIs(.SEMICOLON) and !self.curTokenIs(.EOF)) {
            self.nextToken();
        }
        
        stmt.return_value = null;

        return ast.Statement{ .Return = stmt };
    }
    
    fn parseExpressionStatement(self: *Parser) !?ast.Statement {
        // Placeholder: consume until semicolon to avoid infinite loop
        while (!self.curTokenIs(.SEMICOLON) and !self.curTokenIs(.EOF)) {
            self.nextToken();
        }
        return null;
    }

    fn curTokenIs(self: *Parser, t: TokenType) bool {
        return self.cur_token.type == t;
    }

    fn peekTokenIs(self: *Parser, t: TokenType) bool {
        return self.peek_token.type == t;
    }

    fn expectPeek(self: *Parser, t: TokenType) bool {
        if (self.peekTokenIs(t)) {
            self.nextToken();
            return true;
        } else {
            self.peekError(t);
            return false;
        }
    }

    fn peekError(self: *Parser, t: TokenType) void {
        const msg = std.fmt.allocPrint(self.allocator, "expected next token to be {}, got {} instead", .{t, self.peek_token.type}) catch return;
        self.errors.append(msg) catch return;
    }
};
