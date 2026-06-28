---
project: 42Minishell
date: 2026-06-09
scope: mandatory
---

# Remaining Mandatory Work

## Delivery Rule

Mandatory work ships before bonus work. MSH-18 and MSH-19 remain parked until MSH-10 through MSH-17 are complete, stable, reviewed, and accepted.

## Status Overview

| Jira | Status | Remaining Work |
| --- | --- | --- |
| MSH-10 | In Progress | Complete MSH-25 review, finish MSH-26, then close the parent. |
| MSH-11 | Done | Regression coverage only. |
| MSH-12 | In Progress | Finish expansion for `$VAR`, `$?`, quote rules, missing vars, and integration. |
| MSH-13 | Done | Regression coverage only. |
| MSH-14 | To Do | Implement mandatory builtins and parent-context routing. |
| MSH-15 | To Do | Implement redirections and heredoc. |
| MSH-16 | To Do | Implement multi-command pipelines. |
| MSH-17 | To Do | Final stability, test matrix, memory checks, and README. |

## MSH-10: Interactive Shell Bootstrap and Signal Baseline

Remaining:

- Complete review of PR MSH-25 for `Ctrl-\`.
- Complete MSH-26.
- Confirm the parent still satisfies:
  - prompt display
  - `readline`
  - non-empty history
  - `Ctrl-C` fresh prompt
  - `Ctrl-D` empty-prompt exit
  - `Ctrl-\` no-op at interactive prompt
  - at most one global variable storing only the signal number

## MSH-12: Environment and Exit Status Expansion

Remaining:

- Implement quote-aware expansion.
- Expand `$VAR` from environment.
- Expand `$?` from last foreground status.
- Confirm missing variables expand to empty.
- Preserve argv boundaries for existing single-command execution.
- Verify bash-aligned behavior for mandatory cases.
- Run build and leak checks on success and error paths.

## MSH-14: Mandatory Builtins

Remaining:

- `echo` with `-n`.
- `cd` with relative and absolute path input.
- `pwd` with no options.
- `export` with no options.
- `unset` with no options.
- `env` with no options or arguments.
- `exit` with no options.
- Parent-context behavior for stateful builtins when not in a pipeline.
- Clear errors and statuses aligned with bash mandatory expectations.

## MSH-15: Mandatory Redirections and Heredoc

Remaining:

- Parse and apply `<`.
- Parse and apply `>`.
- Parse and apply `>>`.
- Parse and apply `<<`.
- Read heredoc until delimiter.
- Keep heredoc input out of command history.
- Restore file descriptors after command execution.
- Prevent fd leaks across prompt iterations and later commands.
- Stop affected command on redirection errors with appropriate status reporting.

## MSH-16: Multi-Command Pipelines

Remaining:

- Chain commands with `|`.
- Support more than one pipe pair.
- Close unused pipe fds in parent and child paths.
- Run external commands and builtins correctly in pipeline contexts.
- Update `$?` from the most recent foreground pipeline.
- Verify pipeline behavior with redirections once MSH-15 is complete.

## MSH-17: Mandatory Stability, Test Matrix, and README

Remaining:

- Build a focused mandatory regression matrix:
  - prompt behavior
  - signals
  - tokenization and quote errors
  - expansion
  - builtins
  - redirections
  - heredoc
  - pipelines
  - command failure statuses
  - memory cleanup
- Run `make fclean && make`.
- Run Valgrind on representative success and failure paths.
- Review fd behavior for redirections and pipelines.
- Update root `README.md` with the required intro line, English description, instructions, and resources/AI usage section.
- Capture Jira-ready verification notes for each mandatory subsystem.

## Recommended Order

1. Close remaining MSH-10 items.
2. Finish MSH-12.
3. Implement MSH-14.
4. Implement MSH-15.
5. Implement MSH-16.
6. Complete MSH-17.
7. Only then revisit MSH-18 and MSH-19.

