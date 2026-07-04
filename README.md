*This project has been created as part of the 42 curriculum by lmanuel-, jpedro-g.*

# Minishell

## Description

Minishell is a small Bash-like shell written in C.

The goal of this project is to implement the core behavior of an interactive Unix shell, including command parsing, quote handling, environment expansion, redirections, pipes, builtins, external command execution, exit statuses, and signal handling.

This implementation focuses on the mandatory scope of the 42 Minishell subject.

## Features

- Interactive prompt with command history
- Execution of commands using absolute paths, relative paths, or the `PATH` environment variable
- Single quotes and double quotes
- Environment variable expansion with `$VAR`
- Last exit status expansion with `$?`
- Input, output, append, and heredoc redirections:
  - `<`
  - `>`
  - `>>`
  - `<<`
- Pipes with two or more commands
- Signal handling for `Ctrl-C`, `Ctrl-D`, and `Ctrl-\`
- Builtin commands:
  - `echo` with option `-n`
  - `cd`
  - `pwd`
  - `export`
  - `unset`
  - `env`
  - `exit`

## Instructions

Compile the project:

```sh
make
```

Run Minishell:

```sh
./minishell
```

Clean object files:

```sh
make clean
```

Remove object files and the executable:

```sh
make fclean
```

Rebuild from scratch:

```sh
make re
```

## Usage Examples

```sh
echo hello
echo "$USER"
echo '$USER'
pwd
cd /tmp
export TEST=42
echo $TEST
unset TEST
cat < input.txt | grep hello > output.txt
cat << EOF
hello
EOF
echo $?
```

## Project Scope

This project implements the mandatory part of Minishell.

The following features are intentionally outside the mandatory scope and are not implemented:

 - `&&`
 - `||`
 - Parentheses for command grouping
 - Wildcard expansion with `*`

## Resources

Classic references used during the project:

- 42 Minishell subject
- Bash manual
- Linux manual pages for:
  - `printf`
  - `malloc`
  - `free`
  - `write`
  - `access`
  - `open`
  - `read`
  - `close`
  - `fork`
  - `wait`
  - `waitpid`
  - `sigaction`
  - `sigemptyset`
  - `exit`
  - `getcwd`
  - `chdir`
  - `execve`
  - `dup`
  - `dup2`
  - `pipe`
  - `perror`
- GNU Readline documentation for:
  - `readline`
  - `rl_clear_history`
  - `rl_on_new_line`
  - `rl_replace_line`
  - `rl_redisplay`
  - `add_history`

AI was used as a support tool for planning, debugging strategies, edge-case review, test matrix design, and documentation drafting. The implementation was reviewed, tested, and adapted by the team. The team is responsible for understanding and defending every part of the submitted code.
