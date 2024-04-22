use std::env;
use colored::Colorize;

//Made a simple logger because the log crate is not working for some reason.

pub fn info(msg: &str) {
    println!("{} {}", "[INFO]".green().bold(), msg.green().bold());
}

pub fn warn(msg: &str) {
    println!("{} {}", "[WARN]".yellow().bold(), msg.yellow().bold());
}

pub fn error(msg: &str) {
    println!("{} {}","[ERROR]".red().bold(), msg.red().bold());
}

pub fn verbose(msg: &str) {
    let verbose = env::var("VERBOSE");
    if verbose == Ok("1".to_string()) {
        println!("{} {}","[VERBOSE]".cyan().bold(), msg.cyan().bold());
    }
}