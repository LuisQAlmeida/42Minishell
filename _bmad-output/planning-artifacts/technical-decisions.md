---
project: 42Minishell
date: 2026-06-09
status: living-decisions
---

# Technical Decisions

## TD-001: Mandatory Before Bonus

Decision: Complete and stabilize all mandatory stories before starting MSH-18 or MSH-19.

Reason: Bonus behavior changes parser and expansion semantics. Mixing it into mandatory paths increases review risk and can hide mandatory regressions.

Implication: Wildcards, logical operators, and parentheses are not accepted in MSH-12 through MSH-17 PRs.

## TD-002: Bash Is the Reference for Ambiguous Behavior

Decision: Use bash as the behavior reference when the 42 subject is ambiguous.

Reason: The backlog already defines bash as the mandatory ambiguity reference.

Implication: PR descriptions should include the bash comparison command when behavior is subtle.

## TD-003: One Parent Jira Ticket Equals One Branch and One PR

Decision: Each parent Jira item uses one branch and one GitHub PR.

Reason: Keeps review aligned to delivery scope and avoids cross-ticket partial merges.

Implication: MSH-12 subtasks are commits inside the MSH-12 PR, not separate branches or separate PRs.

## TD-004: Expansion Runs After Quote-Aware Tokenization

Decision: Expansion should consume tokenizer output that preserves quote context.

Reason: `$VAR` and `$?` must expand in unquoted and double-quoted content, but not inside single quotes.

Implication: Avoid implementing expansion as a blind full-line string rewrite.

## TD-005: Shell State Owns Last Status

Decision: `$?` reads from shell state updated after foreground command execution.

Reason: Current execution already tracks single-command status, including status 127 for invalid commands.

Implication: Future pipelines must update the same state with the status of the most recent foreground pipeline.

## TD-006: Builtins That Mutate Shell State Run in Parent Context When Not Pipelined

Decision: `cd`, `export`, `unset`, and `exit` must run in the parent shell context for non-pipeline commands.

Reason: Running these only in a child process would discard required shell state changes.

Implication: MSH-14 should create explicit builtin routing before MSH-16 pipeline isolation.

## TD-007: External Execution Remains Fork/Execve Based

Decision: External commands continue to run through `fork`, `execve`, and `wait` or `waitpid`.

Reason: This is already implemented and verified for one command.

Implication: Expansion and builtins should integrate without replacing the working execution path.

## TD-008: Redirections Own File Descriptor Setup and Restoration

Decision: MSH-15 should make fd ownership explicit for redirection setup, command execution, and restoration.

Reason: Redirection and heredoc errors can otherwise leak modified stdin/stdout into the prompt or later commands.

Implication: Single-command redirection should be stable before multi-command pipeline fd chaining.

## TD-009: Heredoc Input Is Not Added to History

Decision: Heredoc input lines are not recorded as readline command history.

Reason: The backlog acceptance criteria explicitly separates heredoc input from history updates.

Implication: Heredoc reading should not reuse the normal prompt loop history behavior without a guard.

## TD-010: Memory Verification Tracks Shell-Owned Allocations

Decision: Valgrind review focuses on shell-owned allocations while recognizing still-reachable memory from readline/libtinfo.

Reason: Current verified results show no definitely, indirectly, or possibly lost memory on tested paths, with still reachable mostly from external libraries.

Implication: Each mandatory PR should include leak checks for success and error paths it changes.

