const std = @import("std");

pub fn main() !void {
    const stdout = std.io.getStdOut().writer();
    const stdin = std.io.getStdIn().reader();

    try stdout.print("REPLngne (Zig Edition)
", .{});
    try stdout.print("Type 'exit' to quit.
", .{});

    var buffer: [1024]u8 = undefined;

    while (true) {
        try stdout.print(">>> ", .{});
        
        if (try stdin.readUntilDelimiterOrEof(&buffer, '
')) |line| {
            if (std.mem.eql(u8, line, "exit")) {
                break;
            }
            try stdout.print("Echo: {s}
", .{line});
        } else {
            break;
        }
    }
}
