---
sourceDocuments:
  - docs/subject/minishell_subject.pdf
  - docs/project-management/working-agreements.md
status: draft
---

# 42 Minishell Backlog (Mandatory vs Bonus)

## Backlog Rules

- Mandatory ships first and must be fully implementable without bonus features.
- Bonus work stays parked until mandatory is complete, stable, and reviewable.
- When mandatory behavior is ambiguous, use bash as the reference.
- No scope beyond the subject: no unsupported shell syntax, no extra builtin options, no bonus hooks mixed into mandatory code paths.

## Epic 1: Mandatory Shell Delivery

**Goal:** Deliver a subject-compliant interactive minishell that passes mandatory evaluation without depending on any bonus parsing or execution behavior.

### Story M1: Interactive Shell Bootstrap and Signal Baseline

- **Title:** Interactive shell bootstrap and signal baseline
- **Scope:** Mandatory
- **Subsystem:** Signals
- **Acceptance Criteria:**
  - Shell displays a prompt while waiting for input.
  - Interactive input uses `readline` and successful non-empty commands are added to history.
  - `Ctrl-C` in interactive mode prints a new line and redisplays a fresh prompt.
  - `Ctrl-D` exits the shell cleanly from interactive prompt.
  - `Ctrl-\` does nothing in interactive prompt.
  - Signal handling uses at most one global variable, and that variable stores only the signal number.
- **Dependencies:** None
- **Labels:** `jira`, `minishell`, `mandatory`, `signals`, `core-loop`, `subject-v10`

### Story M2: Mandatory Tokenization and Syntax Validation

- **Title:** Tokenize commands with mandatory quoting, pipes, and redirections
- **Scope:** Mandatory
- **Subsystem:** Parser
- **Acceptance Criteria:**
  - Parser recognizes words, pipes, and mandatory redirection operators: `<`, `>`, `<<`, `>>`.
  - Single quotes preserve literal content and suppress metacharacter interpretation.
  - Double quotes preserve literal content except `$` expansion markers.
  - Unclosed quotes are not interpreted as valid executable input.
  - Unsupported special characters required to stay out of scope by the subject are not implemented.
  - Syntax errors for malformed pipe or redirection placement are detected before execution.
- **Dependencies:** M1
- **Labels:** `jira`, `minishell`, `mandatory`, `parser`, `syntax`, `quotes`

### Story M3: Environment and Exit Status Expansion

- **Title:** Expand environment variables and `$?` for mandatory command input
- **Scope:** Mandatory
- **Subsystem:** Expansion
- **Acceptance Criteria:**
  - `$VAR` expands using the current shell environment.
  - `$?` expands to the exit status of the most recently executed foreground pipeline.
  - Expansion occurs inside double quotes and is suppressed inside single quotes.
  - Missing variables expand consistently with bash mandatory expectations.
  - Expansion output integrates with parser/executor data structures without corrupting token boundaries required by mandatory features.
- **Dependencies:** M2
- **Labels:** `jira`, `minishell`, `mandatory`, `expansion`, `env`, `exit-status`

### Story M4: External Command Resolution and Single Command Execution

- **Title:** Resolve executables and run single non-pipeline commands
- **Scope:** Mandatory
- **Subsystem:** Execution
- **Acceptance Criteria:**
  - External commands resolve through `PATH` when invoked by command name.
  - Relative and absolute executable paths are supported.
  - Child processes execute via `fork` and `execve`.
  - Parent process waits for foreground completion and stores resulting exit status.
  - Command-not-found and execution failures return shell-appropriate error reporting and status codes aligned with mandatory expectations.
- **Dependencies:** M1, M2, M3
- **Labels:** `jira`, `minishell`, `mandatory`, `execution`, `path`, `processes`

### Story M5: Mandatory Builtins

- **Title:** Implement subject-required builtins with mandatory-only option support
- **Scope:** Mandatory
- **Subsystem:** Builtins
- **Acceptance Criteria:**
  - `echo` supports `-n`.
  - `cd` supports only relative or absolute path input.
  - `pwd` works with no options.
  - `export` works with no options.
  - `unset` works with no options.
  - `env` works with no options or arguments.
  - `exit` works with no options.
  - Builtins that must affect shell state (`cd`, `export`, `unset`, `exit`) run in the parent context when not isolated by pipeline behavior.
- **Dependencies:** M3, M4
- **Labels:** `jira`, `minishell`, `mandatory`, `builtins`, `env-state`

### Story M6: Mandatory Redirections and Heredoc

- **Title:** Apply input/output redirections including heredoc
- **Scope:** Mandatory
- **Subsystem:** Redirections
- **Acceptance Criteria:**
  - `<` redirects input from a file.
  - `>` redirects output to a file with truncation.
  - `>>` redirects output in append mode.
  - `<<` reads input until the delimiter line is reached.
  - Heredoc input handling does not require history updates.
  - File descriptor setup and restoration prevent leakage into unrelated commands.
  - Redirection errors stop the affected command with appropriate status reporting.
- **Dependencies:** M2, M4
- **Labels:** `jira`, `minishell`, `mandatory`, `redirections`, `heredoc`, `fds`

### Story M7: Multi-Command Pipelines

- **Title:** Execute mandatory pipelines with correct pipe chaining and statuses
- **Scope:** Mandatory
- **Subsystem:** Pipes
- **Acceptance Criteria:**
  - `|` connects the output of each command to the input of the next command.
  - Pipelines support multiple commands, not only a single pipe pair.
  - Pipeline processes close unused file descriptors in both parent and child paths.
  - Builtins and external commands both execute correctly within pipeline contexts.
  - The shell records the exit status of the most recently executed foreground pipeline for `$?`.
- **Dependencies:** M4, M5, M6
- **Labels:** `jira`, `minishell`, `mandatory`, `pipes`, `execution`, `fds`

### Story M8: Mandatory Stability, Test Matrix, and Readme

- **Title:** Harden mandatory behavior with regression tests and required documentation
- **Scope:** Mandatory
- **Subsystem:** Testing
- **Acceptance Criteria:**
  - A focused manual or automated test matrix covers prompt behavior, quoting, expansion, builtins, redirections, pipes, and signals.
  - Mandatory flows are verified without relying on any bonus syntax.
  - Memory ownership is reviewed for shell-authored code paths to avoid leaks, crashes, and double frees.
  - `Makefile` supports mandatory compilation targets required by the subject.
  - Root `README.md` includes the required intro line, English description, instructions, and resources/AI usage section.
  - Jira-ready verification notes define how to test each mandatory subsystem in PR review.
- **Dependencies:** M1, M2, M3, M4, M5, M6, M7
- **Labels:** `jira`, `minishell`, `mandatory`, `testing`, `docs`, `readme`, `release-gate`

## Epic 2: Bonus Features (Parked Until Mandatory Is Stable)

**Goal:** Add bonus-only shell behavior after mandatory is complete, without destabilizing or refactoring the already-complete mandatory path.

**Parking Rule:** Do not start this epic until Epic 1 is complete, stable, and accepted in review.

### Story B1: Logical Operators and Parenthesis Precedence

- **Title:** Implement `&&` and `||` with parenthesis-based priority
- **Scope:** Bonus
- **Subsystem:** Parser
- **Acceptance Criteria:**
  - Parser recognizes `&&` and `||` as bonus-only operators.
  - Parentheses define execution priority for logical expressions.
  - Logical short-circuit behavior matches bash for supported bonus syntax.
  - Invalid logical-expression syntax fails cleanly without breaking mandatory command parsing.
  - Bonus parsing is isolated so mandatory parsing remains fully functional when bonus code is disabled or unfinished.
- **Dependencies:** Epic 1 complete, M2, M4, M7
- **Labels:** `jira`, `minishell`, `bonus`, `parser`, `execution`, `logical-operators`, `parked`

### Story B2: Wildcard Expansion in Current Working Directory

- **Title:** Implement `*` wildcard expansion for current directory matches
- **Scope:** Bonus
- **Subsystem:** Expansion
- **Acceptance Criteria:**
  - `*` expands against entries in the current working directory only.
  - Expansion integrates with command arguments without changing mandatory variable expansion behavior.
  - Non-matching or edge-case handling is validated against bash as reference for the supported scope.
  - Hidden-file behavior is explicitly tested and documented according to chosen bash-aligned behavior.
  - Wildcard support is isolated so mandatory command handling remains unchanged when bonus is not enabled.
- **Dependencies:** Epic 1 complete, M3, M4
- **Labels:** `jira`, `minishell`, `bonus`, `expansion`, `wildcards`, `parked`

## Delivery Notes

- Recommended implementation order: M1, M2, M3, M4, M5, M6, M7, M8, then B1, B2.
- Branching and review should follow the working agreement: short-lived Jira-key branches, PR-based merges, and mandatory-first sequencing.
- Bonus stories should stay out of active sprint scope until mandatory acceptance is achieved.
