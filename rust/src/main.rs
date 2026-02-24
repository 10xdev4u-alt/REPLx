mod lexer;
mod token;

use rustyline::error::ReadlineError;
use rustyline::DefaultEditor;
use crate::lexer::Lexer;
use crate::token::Token;

fn main() -> rustyline::Result<()> {
    let mut rl = DefaultEditor::new()?;
    
    println!("REPLngne (Rust Edition) - v0.1.0");
    println!("Type 'exit' or Ctrl+D to quit");

    loop {
        let readline = rl.readline(">> ");
        match readline {
            Ok(line) => {
                rl.add_history_entry(line.as_str())?;
                
                let mut lexer = Lexer::new(line);
                
                loop {
                    let tok = lexer.next_token();
                    if tok == Token::EOF {
                        break;
                    }
                    println!("{:?}", tok);
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
