# Minishell Architecture

## Purpose

This document describes a simple, evaluator-friendly architecture for 42 Minishell.
It is mandatory-first: all core design choices support the mandatory subject only.
Bonus features stay isolated and must not complicate or block the mandatory path.

## Sources

- `docs/subject/minishell_subject.pdf`
- `_bmad-output/planning-artifacts/minishell-backlog-mandatory-vs-bonus.md`
- `docs/architecture/decisions.md`

## Design Principles

- Build only what the subject requires.
- Prefer a linear pipeline model over a complex AST for mandatory scope.
- Keep parsing, expansion, and execution separated by clear data handoff.
- Keep bonus parsing and wildcard logic out of mandatory execution paths.
- When mandatory behavior is ambiguous, compare with bash and match the observed result.

## Mandatory Scope Boundary

Mandatory architecture covers:

- Interactive prompt and history
- Tokenization for words, quotes, pipes, and redirections
- Parsing into a pipeline of commands
- Environment and `$?` expansion
- Builtins required by the subject
- External command execution
- Redirections and heredoc
- Pipelines
- Signal handling with one global signal number only

Mandatory architecture does not include:

- `&&`
- `||`
- Parentheses
- Wildcard `*` expansion
- Extra shell syntax such as `;` or `\`

## End-to-End Flow

The mandatory runtime flow is:

`readline -> tokenize -> parse -> expand -> execute`

### 1. Readline

- Main loop displays the prompt and reads one input line with `readline`.
- If the line is `NULL`, treat it as `Ctrl-D` and exit cleanly.
- If the line is non-empty, add it to history.
- The line is owned by the current iteration and freed before the next prompt.

### 2. Tokenize

- Convert the raw line into a flat token stream.
- Recognize:
  - words
  - quoted segments
  - `|`
  - `<`
  - `>`
  - `<<`
  - `>>`
- Tokenizer validates unclosed quotes early.
- Tokenizer does not try to understand command meaning; it only produces lexical units.

### 3. Parse

- Convert tokens into a mandatory command model:
  - a pipeline list
  - each pipeline entry is one command node
  - each command node contains argv and redirections
- Parser validates syntax such as:
  - missing command after a pipe
  - missing file/delimiter after a redirection
  - invalid operator placement
- The parser output is the first structured representation used by execution.

### 4. Expand

- Expansion runs on parsed command words and redirection operands where mandatory behavior requires it.
- Expand:
  - `$VAR`
  - `$?`
- Respect quote context:
  - single quotes: no expansion
  - double quotes: variable expansion allowed
- Expansion produces final argument strings for execution.
- Mandatory expansion does not perform wildcard expansion.

### 5. Execute

- Executor receives a fully parsed and expanded pipeline.
- It decides:
  - builtin vs external command
  - parent execution vs child execution
  - redirection setup
  - pipe creation and closure
- For external commands, resolve path then `fork` and `execve`.
- For foreground jobs, parent waits and stores the last pipeline exit status for `$?`.

## Core Runtime Model

The simplest evaluator-friendly model is a command-list pipeline, not an AST.
Mandatory syntax does not need operator precedence beyond left-to-right pipelines and per-command redirections.

## Key Structs and Responsibilities

Names can change in code, but the responsibilities should stay stable.

### `t_shell`

Top-level runtime state for the current shell process.

Suggested responsibilities:

- environment list or environment array source of truth
- last exit status
- interactive mode state
- saved original `stdin`/`stdout` if needed for restoration
- pointers to current parse/execution objects for one loop iteration

This struct must not be accessed directly from signal handlers.

### `t_token`

One lexical unit produced by tokenization.

Suggested fields:

- token type
- raw string value
- quote context flags if useful
- linked-list or array next position

Responsibilities:

- represent raw command line pieces before syntax grouping

### `t_redir`

One redirection attached to a command.

Suggested fields:

- redirection type (`IN`, `OUT`, `APPEND`, `HEREDOC`)
- target string (filename or heredoc delimiter)
- resolved heredoc fd or temporary source handle when needed

Responsibilities:

- store redirection intent separately from `argv`
- give executor a simple list to apply in order

### `t_command`

One command inside a pipeline.

Suggested fields:

- `argv`
- builtin kind or command classification
- redirection list
- command-specific execution metadata

Responsibilities:

- represent one executable step
- keep command arguments separate from file descriptor setup data

### `t_pipeline`

The parsed command sequence for one input line.

Suggested fields:

- ordered list/array of `t_command`
- command count

Responsibilities:

- define left-to-right pipeline execution order
- provide the execution unit whose final status updates `$?`

### `t_env`

Environment variable storage node if using a linked list.

Suggested fields:

- key
- value
- export flag if needed

Responsibilities:

- support `env`, `export`, `unset`, lookup for expansion, and building `envp` for `execve`

## Memory Ownership Rules

The mandatory goal is predictable ownership and easy cleanup per loop iteration.

### General Rules

- Each allocation has one clear owner.
- Ownership moves only at explicit handoff points.
- On parse or execution error, free the whole current iteration in one cleanup path.
- Ignore `readline` internal leaks allowed by the subject, but free all shell-owned memory.

### Ownership by Stage

- Readline owns the raw line until tokenization completes; then the shell loop remains responsible for freeing it.
- Tokenizer owns token allocations until parse succeeds; after parse, tokens can be freed immediately or retained until parse cleanup, but the rule must be consistent.
- Parser owns command, argv, and redirection structures for the current iteration.
- Expansion replaces or updates command strings; old replaced strings must be freed by the expansion stage before storing new ones.
- Executor owns only temporary runtime resources:
  - child pids
  - pipe fd arrays
  - heredoc temp fds
  - optional `envp` arrays for `execve`
- Parent shell state (`t_shell`, environment store, last status) survives across loop iterations.

### Cleanup Boundaries

- Per command line:
  - raw input
  - token list
  - parsed pipeline
  - temporary heredoc resources
  - temporary `envp` arrays
- Persistent:
  - shell struct
  - environment store
  - readline/session state until shell exit

## Redirection and Pipe FD Lifecycle

File descriptor handling must stay explicit and short-lived.

### Redirections

Per command:

1. Parse stores redirection operations in source order.
2. Before executing the command, open required files or prepare heredoc input.
3. Apply redirections with `dup2`.
4. Close original temporary fds after duplication.
5. Run builtin or external command in the chosen context.
6. Restore parent fds if the command ran in the parent process.

Rules:

- Redirection failure cancels execution of that command.
- If multiple redirections affect the same stream, the last effective one wins because they are applied in order.
- Heredoc should end with a readable fd for the command's `stdin`.

### Pipes

For a pipeline of `N` commands:

1. Create `N - 1` pipes.
2. For each command:
   - connect previous pipe read end to `stdin` when not first
   - connect current pipe write end to `stdout` when not last
   - apply command-specific redirections after pipe planning
3. In each child, close all unused pipe fds immediately after `dup2`.
4. In the parent, close pipe ends as soon as they are no longer needed.
5. After spawning the pipeline, wait for all children and record the final pipeline status.

Practical rule:

- Never leave pipe fds open in unrelated processes, or downstream commands may never see EOF.

## Signal Strategy

Mandatory signal handling must obey the subject constraint:

- use at most one global variable
- that global variable stores only the received signal number

### Strategy

- Define one global `volatile sig_atomic_t` signal number.
- Signal handlers do minimal work:
  - store the signal number
  - optionally write a newline only if that behavior is safe and deliberately chosen
- Main control flow checks the global flag at safe points:
  - after `readline` returns
  - before rendering the next prompt
  - after waiting on children
- Main loop converts that signal state into shell behavior:
  - prompt refresh for interactive `Ctrl-C`
  - exit status updates when appropriate

### Why This Matters

- Signal handlers must not touch parser state, token lists, command lists, or environment structures.
- This keeps async behavior simple, norm-friendly, and easy to defend during evaluation.

## Builtin Execution Context

Builtins do not all run in the same place.
The execution context depends on whether shell state must persist and whether the command is in a pipeline.

### Run in Parent Process

Use parent execution for standalone builtins that must modify shell state:

- `cd`
- `export`
- `unset`
- `exit`

Reason:

- Their effects must survive after the command finishes.

These may still need temporary redirections in the parent:

- save original fd
- apply redirections
- run builtin
- restore original fd

### Can Run in Child Process

These can safely run in a child when needed:

- `echo`
- `pwd`
- `env`

They do not need to mutate persistent shell state.

### Builtins Inside Pipelines

When a builtin is part of a pipeline, treat it as a pipeline stage:

- execute it in a child process
- connect it to pipe fds like any other command

Reason:

- the pipeline requires concurrent process-style data flow
- side effects from `cd`, `export`, `unset`, or `exit` must not incorrectly mutate the parent shell when they are inside a pipeline

This keeps pipeline semantics simple and matches expected shell behavior for the subject scope.

## Error Handling Model

- Syntax errors stop before execution.
- Redirection/open errors stop the affected command or pipeline setup cleanly.
- `execve` failures report an error and set a meaningful exit status.
- Builtin argument validation stays limited to mandatory scope only.
- Fatal internal errors must still clean up shell-owned resources before exit when possible.

## Suggested Module Split

Keep files small and responsibilities obvious.

- `main` / shell loop
- `signals`
- `lexer` or `tokenizer`
- `parser`
- `expansion`
- `builtins`
- `executor`
- `redir`
- `pipe`
- `env`
- `cleanup`
- `utils`

This is a guideline, not a requirement. The main goal is clear separation, not a specific folder layout.

## Bonus Impact

This section describes what would change for bonus features without implementing them in the mandatory architecture.

### `&&` and `||`

Impact:

- The parser can no longer treat the whole input as only a simple pipeline list.
- A higher-level execution structure is needed to represent conditional chaining.
- The executor must short-circuit:
  - `&&` runs the right side only when the left side succeeds
  - `||` runs the right side only when the left side fails

Isolation plan:

- Keep the current mandatory parser entry point for simple pipelines.
- Add a separate bonus parser layer above pipeline parsing.
- Reuse mandatory pipeline execution as the leaf execution unit.

### Parentheses `()`

Impact:

- Parser must support grouped expressions and precedence.
- Tokenizer must recognize `(` and `)` as meaningful operators in bonus mode.
- Execution model needs nested structures rather than only a flat pipeline.

Isolation plan:

- Do not alter mandatory command-node layout.
- Introduce a bonus expression node type that can contain sub-expressions and pipelines.

### Wildcard `*`

Impact:

- Expansion stage gains pathname matching against the current working directory.
- Expansion must decide when to replace one token with many arguments.
- Matching behavior must be aligned with bash for the supported scope.

Isolation plan:

- Add wildcard expansion as a bonus-only pass inside the expansion module.
- Run it after mandatory variable expansion logic, without changing mandatory tokenization rules.

## Evaluator Notes

- This design is intentionally simple because the mandatory subject is intentionally limited.
- The chosen model is easier to explain and safer to implement than a full shell grammar.
- Bonus remains a clean extension point, not a hidden dependency of mandatory code.
