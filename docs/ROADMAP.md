# REPLngne Roadmap

## Phase 1: Foundation (Current)
- [x] Create project structure
- [x] Document high-level architecture
- [x] Research language-specific strategies
- [ ] Initialize project boilerplates for all 4 languages

## Phase 2: Core Loop (The Minimal Viable REPL)
**Goal:** A simple loop that reads a line, prints it back, and repeats.
- [ ] **Rust**: Implement `rustyline` loop with signal handling.
- [ ] **Go**: Implement `readline` loop or `bufio.Scanner`.
- [ ] **C**: Implement `linenoise` or raw `fgets` loop.
- [ ] **Zig**: Implement `std.io.getStdIn()` loop.

## Phase 3: The Calculator (Parsing & Eval)
**Goal:** Parse and evaluate basic arithmetic expressions (`1 + 2 * 3`).
- [ ] Lexer: Tokenize integers, `+`, `-`, `*`, `/`, `(`, `)`.
- [ ] Parser: Recursive descent parser for operator precedence.
- [ ] Evaluator: Tree-walk interpreter.

## Phase 4: State & Variables
**Goal:** Support `let x = 10;` and `x + 5`.
- [ ] Environment: Hash map implementation for storing variable bindings.
- [ ] Scope: Nested environments for block scoping (if/else, functions).

## Phase 5: Control Flow & Functions
**Goal:** Support `if`, `while`, and function definitions.
- [ ] AST Nodes: `IfExpression`, `WhileLoop`, `FunctionLiteral`.
- [ ] Evaluator: Stack frame management for function calls.

## Phase 6: Performance Optimization
**Goal:** JIT compilation or Bytecode VM.
- [ ] **Rust**: Explore `cranelift` for JIT?
- [ ] **Go**: Optimize allocation patterns.
- [ ] **C**: Custom allocator for AST nodes.
- [ ] **Zig**: Comptime optimizations for parser tables.

## Phase 7: Polish & UI
- [ ] Syntax Highlighting
- [ ] Auto-completion
- [ ] History persistence
