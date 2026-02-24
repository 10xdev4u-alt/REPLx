mod lexer;
mod token;
mod ast;
mod parser;

use rustyline::error::ReadlineError;
use rustyline::DefaultEditor;
use crate::lexer::Lexer;
use crate::parser::Parser;
use crate::ast::Node;

fn main() -> rustyline::Result<()> {
    let mut rl = DefaultEditor::new()?;
    
    println!("REPLngne (Rust Edition) - v0.2.0 (Parser Active)");
    println!("Type 'exit' or Ctrl+D to quit");

    loop {
        let readline = rl.readline(">> ");
        match readline {
            Ok(line) => {
                rl.add_history_entry(line.as_str())?;
                
                let lexer = Lexer::new(line);
                let mut parser = Parser::new(lexer);
                
                let program = parser.parse_program();
                
                if parser.errors.len() > 0 {
                    println!("Parser Errors:");
                    for msg in parser.errors {
                        println!("\t{}", msg);
                    }
                    continue;
                }

                if let Some(prog) = program {
                    println!("{}", prog.string());
                }
            },
            Err(ReadlineError::Interrupted) => {
                println!("CTRL-C");
                break;
            },
            Err(ReadlineError::Eof) => {
                println!("CTRL-D");
                break;
            },
            Err(err) => {
                println!("Error: {:?}", err);
                break;
            }
        }
    }
    Ok(())
}
