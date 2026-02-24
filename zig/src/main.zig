const std = @import("std");
const Lexer = @import("lexer.zig").Lexer;
const Token = @import("token.zig").Token;
const TokenType = @import("token.zig").TokenType;

pub fn main() !void {
    const stdout = std.io.getStdOut().writer();
    const stdin = std.io.getStdIn().reader();

    try stdout.print("REPLngne (Zig Edition) - v0.1.0\n", .{});
    try stdout.print("Type 'exit' to quit.\n", .{});

    var buffer: [1024]u8 = undefined;

    while (true) {
        try stdout.print(">>> ", .{});
        
        if (try stdin.readUntilDelimiterOrEof(&buffer, '\n')) |line| {
            // Trim CR if present (Windows line endings)
            var trimmed_line = line;
            if (std.mem.endsWith(u8, line, "\r")) {
                trimmed_line = line[0 .. line.len - 1];
            }

            if (std.mem.eql(u8, trimmed_line, "exit")) {
                break;
            }

            var lexer = Lexer.new(trimmed_line);
            
            while (true) {
                const tok = lexer.nextToken();
                if (tok.type == .EOF) {
                    break;
                }
                // Print token using generated format
                try stdout.print("Type: {any}, Literal: {s}\n", .{tok.type, tok.literal});
            }

        } else {
            break;
        }
    }
}
