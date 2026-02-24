const std = @import("std");
const Token = @import("token.zig").Token;

pub const Node = union(enum) {
    Program: *Program,
    Statement: *Statement,
    Expression: *Expression,
    
    pub fn tokenLiteral(self: Node) []const u8 {
        switch (self) {
            .Program => |p| return p.tokenLiteral(),
            .Statement => |s| return s.tokenLiteral(),
            .Expression => |e| return e.tokenLiteral(),
        }
    }
    
    pub fn string(self: Node, allocator: std.mem.Allocator) ![]u8 {
        switch (self) {
            .Program => |p| return p.string(allocator),
            .Statement => |s| return s.string(allocator),
            .Expression => |e| return e.string(allocator),
        }
    }
};

pub const Statement = union(enum) {
    Let: *LetStatement,
    Return: *ReturnStatement,
    Expression: *ExpressionStatement,
    
    pub fn tokenLiteral(self: Statement) []const u8 {
        switch (self) {
            .Let => |s| return s.token.literal,
            .Return => |s| return s.token.literal,
            .Expression => |s| return s.token.literal,
        }
    }

    pub fn string(self: Statement, allocator: std.mem.Allocator) ![]u8 {
        switch (self) {
            .Let => |s| return s.string(allocator),
            .Return => |s| return s.string(allocator),
            .Expression => |s| return s.string(allocator),
        }
    }
};

pub const Expression = union(enum) {
    Identifier: *Identifier,
    
    pub fn tokenLiteral(self: Expression) []const u8 {
        switch (self) {
            .Identifier => |i| return i.token.literal,
        }
    }

    pub fn string(self: Expression, allocator: std.mem.Allocator) ![]u8 {
        switch (self) {
            .Identifier => |i| return i.string(allocator),
        }
    }
};

pub const Program = struct {
    statements: std.ArrayList(Statement),

    pub fn init(allocator: std.mem.Allocator) Program {
        return Program{
            .statements = std.ArrayList(Statement).init(allocator),
        };
    }

    pub fn deinit(self: *Program) void {
        self.statements.deinit();
    }

    pub fn tokenLiteral(self: *Program) []const u8 {
        if (self.statements.items.len > 0) {
            return self.statements.items[0].tokenLiteral();
        } else {
            return "";
        }
    }

    pub fn string(self: *Program, allocator: std.mem.Allocator) ![]u8 {
        var list = std.ArrayList(u8).init(allocator);
        defer list.deinit();

        for (self.statements.items) |stmt| {
            const s = try stmt.string(allocator);
            defer allocator.free(s);
            try list.appendSlice(s);
        }
        return list.toOwnedSlice();
    }
};

pub const LetStatement = struct {
    token: Token,
    name: *Identifier,
    value: ?Expression, // Optional expression

    pub fn string(self: *LetStatement, allocator: std.mem.Allocator) ![]u8 {
        var list = std.ArrayList(u8).init(allocator);
        defer list.deinit();

        try list.appendSlice(self.token.literal);
        try list.appendSlice(" ");
        
        const nameStr = try self.name.string(allocator);
        defer allocator.free(nameStr);
        try list.appendSlice(nameStr);
        
        try list.appendSlice(" = ");
        
        if (self.value) |val| {
            const valStr = try val.string(allocator);
            defer allocator.free(valStr);
            try list.appendSlice(valStr);
        }
        
        try list.appendSlice(";");
        return list.toOwnedSlice();
    }
};

pub const ReturnStatement = struct {
    token: Token,
    return_value: ?Expression,

    pub fn string(self: *ReturnStatement, allocator: std.mem.Allocator) ![]u8 {
        var list = std.ArrayList(u8).init(allocator);
        defer list.deinit();

        try list.appendSlice(self.token.literal);
        try list.appendSlice(" ");
        
        if (self.return_value) |val| {
            const valStr = try val.string(allocator);
            defer allocator.free(valStr);
            try list.appendSlice(valStr);
        }
        
        try list.appendSlice(";");
        return list.toOwnedSlice();
    }
};

pub const ExpressionStatement = struct {
    token: Token,
    expression: ?Expression,

    pub fn string(self: *ExpressionStatement, allocator: std.mem.Allocator) ![]u8 {
        if (self.expression) |expr| {
            return expr.string(allocator);
        }
        return allocator.dupe(u8, "");
    }
};

pub const Identifier = struct {
    token: Token,
    value: []const u8,

    pub fn string(self: *Identifier, allocator: std.mem.Allocator) ![]u8 {
        return allocator.dupe(u8, self.value);
    }
};
