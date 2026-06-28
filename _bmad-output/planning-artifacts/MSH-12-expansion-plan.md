---
project: 42Minishell
jira: MSH-12
title: Environment and Exit Status Expansion
status: in-progress
scope: mandatory
date: 2026-06-09
---

# MSH-12 Expansion Plan

## Goal

Implement mandatory expansion for `$VAR` and `$?` using bash as the reference when behavior is ambiguous, without introducing bonus behavior or destabilizing the completed tokenizer and single-command executor.

## Scope

In scope:

- Expand `$VAR` from the current shell environment.
- Expand `$?` from the most recently executed foreground command or pipeline state available today.
- Expand inside double quotes.
- Suppress expansion inside single quotes.
- Treat missing variables as empty.
- Preserve mandatory token boundaries expected by the parser and executor.
- Keep the implementation compatible with later builtins, redirections, and pipelines.

Out of scope:

- Wildcard expansion.
- Logical operators.
- Parentheses.
- Brace expansion.
- Command substitution.
- Arithmetic expansion.
- Field splitting beyond what mandatory parsing requires.
- Bonus behavior in mandatory code paths.

## Current Dependencies

Completed foundations:

- MSH-11 tokenizer handles plain words, single quotes, double quotes, and unclosed quote rejection.
- MSH-13 executor accepts parsed single-command argv and stores command status.
- Invalid commands currently return status 127 and the shell remains alive.

Open dependency:

- MSH-10 remains In Progress because Ctrl-\ review and MSH-26 are still open, but it does not block MSH-12 unless signal-state changes alter the shell state struct.

## Required Design Decisions

1. Expansion must run after tokenization has preserved quote context and before final argv is executed.
2. Token data must retain enough quote metadata to distinguish:
   - single-quoted segments: no expansion
   - double-quoted segments: expansion allowed
   - unquoted segments: expansion allowed
3. `$?` must read from the shell state field that is updated by the executor.
4. Expansion should return allocated strings with clear ownership by the caller, matching existing cleanup patterns.
5. Expansion failures caused by allocation errors should stop execution of the current command and return control to the prompt.

## Subtasks as Commits Inside the MSH-12 Parent PR

| Commit | Subtask | Acceptance |
| --- | --- | --- |
| 1 | Add expansion interface | Provide a small expansion API that accepts token data plus shell state and returns expanded token/argv data with documented ownership. |
| 2 | Implement `$?` expansion | `$?` expands to the last stored status before the current command executes. Confirm `badcmd` then `/bin/echo $?` prints `127`. |
| 3 | Implement `$VAR` lookup | Existing environment names expand from `envp`; missing names expand to empty. |
| 4 | Enforce quote rules | Single quotes suppress expansion; double quotes and unquoted text allow expansion. |
| 5 | Integrate with parser/executor | Existing single-command external execution continues to work with expanded argv. |
| 6 | Add focused manual checks | Document and run mandatory expansion cases, including missing vars, quoted vars, and exit status. |
| 7 | Leak and regression pass | Re-run `make fclean && make` and Valgrind on representative expansion paths. |

## Acceptance Criteria

- `$USER` or another known env variable expands in an executable command.
- `$?` reflects the last foreground command status.
- Expansion occurs inside double quotes.
- Expansion does not occur inside single quotes.
- Missing variables expand to empty without crashing.
- Existing tokenization behavior remains unchanged for plain words and quotes.
- Existing single external command execution still passes for PATH, absolute path, and direct executable path cases.
- Command-not-found still returns status 127.
- The shell returns to the prompt after expansion errors or invalid commands.
- Tested paths remain free of shell-owned leaks.

## Suggested Review Script

Run after building:

```sh
make fclean && make
./minishell
```

Manual checks inside minishell:

```sh
/bin/echo $PATH
/bin/echo "$PATH"
/bin/echo '$PATH'
not_a_real_command
/bin/echo $?
/bin/echo "$?"
/bin/echo $MISSING_MINISHELL_VAR
/bin/echo pre$MISSING_MINISHELL_VARpost
/bin/echo pre"$MISSING_MINISHELL_VAR"post
```

Expected behavior should be compared against bash for mandatory syntax only.

## Handoff Notes

- Do not refactor tokenization broadly unless required to preserve quote context.
- Keep one parent branch and one PR for MSH-12.
- Treat each subtask above as a commit in that parent PR.
- Do not include builtins, redirections, pipes, wildcards, logical operators, or parentheses in this PR.

