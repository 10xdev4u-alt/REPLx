# REPL Research & Language Strategy

This document explores the research and specific implementation strategies for building the **REPLngne** in each of the four target languages.

## 1. Rust (Performance + Safety)

### 🧐 Challenges
*   **Ownership Model:** Building recursive data structures like ASTs can be tricky with Rust's strict ownership rules.
*   **Startup Time:** Binaries can be large and compilation slow, but runtime performance is top-tier.

### 🛠 Strategy
*   **Parser:** Use `logos` for the lexer (extreme performance) and a custom Recursive Descent parser.
*   **AST:** Use `Box<T>` or `Rc<RefCell<T>>` for tree nodes where shared ownership is needed.
*   **Evaluator:** A stack-based virtual machine (VM) approach using an `enum` for opcodes.
*   **Input Handling:** `rustyline` (Readline clone in Rust) for history and editing.

## 2. Go (Simplicity + Concurrency)

### 🧐 Challenges
*   **Garbage Collection:** While fast, GC pauses can theoretically affect latency in extremely tight loops, though negligible for a typical REPL.
*   **Error Handling:** Verbose `if err != nil` checks can clutter the parser logic.

### 🛠 Strategy
*   **Parser:** Hand-written recursive descent parser for maximum control.
*   **Concurrency:** Use goroutines for background tasks (e.g., syntax highlighting, auto-complete suggestions).
*   **Input Handling:** `github.com/chzyer/readline` or a custom TTY wrapper.

## 3. C (Control + Speed)

### 🧐 Challenges
*   **Memory Management:** Manual `malloc`/`free` is error-prone. Memory leaks in a long-running REPL session are a serious concern.
*   **String Handling:** C strings are notoriously difficult to manipulate safely.

### 🛠 Strategy
*   **Memory:** Implement a simple "Arena Allocator" or "Region-based Memory Management". Free the entire arena at the end of each evaluation cycle to prevent fragmentation.
*   **Input Handling:** `linenoise` (a minimal, zero-config readline replacement) is perfect here. It supports history and completion without the bloat of GNU Readline.
*   **Data Structures:** Custom dynamic array implementation for tokens and AST nodes.

## 4. Zig (Modern + Explicit)

### 🧐 Challenges
*   **Explicit Allocators:** Passing allocators everywhere can be verbose but forces awareness of memory usage.
*   **Comptime:** Leveraging Zig's compile-time execution for optimization is powerful but requires careful design.

### 🛠 Strategy
*   **Memory:** Use `std.heap.GeneralPurposeAllocator` for development (leak detection!) and switch to `std.heap.ArenaAllocator` for the REPL loop for fast teardown.
*   **Error Handling:** Zig's error sets make handling parser errors clean and efficient.
*   **Input Handling:** Custom raw mode implementation using `std.os` calls for maximum learning, or a lightweight wrapper.

## Comparative Benchmarking Goals

We aim to measure:
1.  **Startup Latency:** Time from command to prompt.
2.  **Execution Speed:** Time to evaluate a tight loop (e.g., Fibonacci recursive).
3.  **Memory Footprint:** Resident Set Size (RSS) after 10,000 operations.
