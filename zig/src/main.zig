const std = @import("std");
const Lexer = @import("lexer.zig").Lexer;
const Token = @import("token.zig").Token;
const TokenType = @import("token.zig").TokenType;
const Parser = @import("parser.zig").Parser;

pub fn main() !void {
    const stdout = std.io.getStdOut().writer();
    const stdin = std.io.getStdIn().reader();

    try stdout.print("REPLngne (Zig Edition) - v0.2.0 (Parser Active)\n", .{});
    try stdout.print("Type 'exit' to quit.\n", .{});

    var buffer: [1024]u8 = undefined;

    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    while (true) {
        try stdout.print(">>> ", .{});
        
        if (try stdin.readUntilDelimiterOrEof(&buffer, '\n')) |line| {
            // Trim CR if present (Windows line endings)
            var trimmed_line = line;
            if (std.mem.endsWith(u8, line, "\r")) {
                trimmed_line = line[0 .. line.len - 1];
            }
            if (std.mem.endsWith(u8, trimmed_line, "\r")) {
                trimmed_line = trimmed_line[0 .. trimmed_line.len - 1];
            }

            if (std.mem.eql(u8, trimmed_line, "exit")) {
                break;
            }

            // Create Arena for this REPL cycle
            var arena = std.heap.ArenaAllocator.init(allocator);
            defer arena.deinit();
            const arena_allocator = arena.allocator();

            const lexer = Lexer.new(trimmed_line);
            var parser = Parser.init(lexer, arena_allocator);
            
            var program = try parser.parseProgram();
            
            if (parser.errors.items.len > 0) {
                try stdout.print("Parser errors:\n", .{});
                for (parser.errors.items) |msg| {
                    try stdout.print("\t{s}\n", .{msg});
                }
            } else {
                const progStr = try program.string();
                try stdout.print("{s}\n", .{progStr});
            }

        } else {
            break;
        }
    }
}
