# Sprint 2

## Sprint Goal

Complete the next mandatory shell slice:

- environment and exit-status expansion
- mandatory builtins
- redirections and heredoc groundwork if capacity allows after expansion and builtins are reviewable

Sprint 2 remains mandatory-only. Bonus tickets MSH-18 and MSH-19 stay parked.

## Current Entry State

Done:

- MSH-11 Mandatory Tokenization and Syntax Validation.
- MSH-13 External Command Resolution and Single Command Execution.

In progress:

- MSH-10 Interactive Shell Bootstrap and Signal Baseline because MSH-25 is in review and MSH-26 remains open.
- MSH-12 Environment and Exit Status Expansion.

To do:

- MSH-14 Mandatory Builtins.
- MSH-15 Mandatory Redirections and Heredoc.
- MSH-16 Multi-Command Pipelines.
- MSH-17 Mandatory Stability, Test Matrix, and README.

## Selected Stories

### MSH-12: Environment and Exit Status Expansion

Included this sprint:

- `$VAR` expansion from the shell environment.
- `$?` expansion from the last foreground command status.
- Expansion inside double quotes.
- No expansion inside single quotes.
- Missing variable behavior aligned with bash mandatory expectations.
- Integration with existing tokenization, parsing, and single-command execution.

Excluded this sprint:

- Wildcards.
- Logical operators.
- Parentheses.
- Command substitution.
- Arithmetic expansion.

### MSH-14: Mandatory Builtins

Included this sprint:

- `echo` with `-n`.
- `cd` with relative or absolute path input.
- `pwd` with no options.
- `export` with no options.
- `unset` with no options.
- `env` with no options or arguments.
- `exit` with no options.
- Parent-context execution for stateful builtins when not in a pipeline.

Excluded this sprint:

- Non-subject options.
- Bash-compatible advanced option parsing outside mandatory requirements.
- Pipeline-specific builtin isolation if MSH-16 has not started.

### MSH-15: Mandatory Redirections and Heredoc, Optional Sprint 2 Pull-In

Pull this story only after MSH-12 and MSH-14 are reviewable.

Included if pulled:

- `<`, `>`, `>>`, and `<<` token handling through existing mandatory syntax structures.
- File open behavior and error reporting.
- fd setup and restoration for a single command.
- Heredoc delimiter reading.

Excluded if pulled:

- Multi-command pipeline fd chaining.
- Bonus syntax.

## Task Split

### Luís

Primary ownership:

- shell state integration
- last-status behavior
- parent-context builtin execution
- prompt-loop behavior after expansion and builtin errors

Concrete tasks:

- Wire `$?` to the existing last-status field.
- Keep prompt behavior stable after expansion and builtin failures.
- Implement or integrate `cd`, `export`, `unset`, and `exit` parent-context routing.
- Verify `Ctrl-C`, `Ctrl-D`, and pending `Ctrl-\` behavior still match mandatory expectations.

### João

Primary ownership:

- expansion engine
- env lookup
- argv integration
- non-stateful builtins and external-command regression checks

Concrete tasks:

- Implement `$VAR` expansion with quote awareness.
- Preserve token boundaries required by the parser and executor.
- Implement or integrate `echo`, `pwd`, and `env`.
- Maintain PATH, absolute path, direct executable path, and command-not-found behavior.

### Shared Integration

- Agree on the expansion API and ownership before coding.
- Pair on stateful builtin review because these affect future pipelines.
- Run bash comparison checks for ambiguous mandatory behavior.
- Update PR descriptions with exact manual verification commands.

## Integration Order

1. Close or account for remaining MSH-10 review items without changing MSH-12 scope.
2. Finish MSH-12 expansion API and `$?` support.
3. Add `$VAR` lookup and quote-rule coverage.
4. Integrate expanded argv into current single-command execution.
5. Implement mandatory builtins in MSH-14.
6. Add parent-context routing for stateful builtins.
7. Pull MSH-15 only if MSH-12 and MSH-14 are reviewable and stable.

## Risks and Mitigations

### Risk 1: Expansion changes token boundaries

- **Impact:** commands execute with incorrect argv.
- **Mitigation:** preserve quote context from tokenizer through expansion; compare representative cases against bash.

### Risk 2: Builtins mutate state in the wrong process

- **Impact:** `cd`, `export`, `unset`, or `exit` appear to run but do not affect the shell.
- **Mitigation:** route stateful builtins through parent context when there is no pipeline.

### Risk 3: Redirection fd handling leaks into later commands

- **Impact:** prompt or subsequent commands inherit wrong input/output.
- **Mitigation:** if MSH-15 is pulled in, implement fd save/restore before broad heredoc behavior.

### Risk 4: Bonus behavior slips into mandatory

- **Impact:** review complexity and unstable mandatory paths.
- **Mitigation:** reject MSH-18 and MSH-19 work during Sprint 2. Any wildcard or logical-operator behavior belongs in a parked future sprint.

## Demo Scenarios

Use these for Sprint 2 review.

1. Run `/bin/echo $PATH`.
2. Run `/bin/echo "$PATH"`.
3. Run `/bin/echo '$PATH'`.
4. Run `not_a_real_command`, then `/bin/echo $?`.
5. Run `echo -n hello` if builtin `echo` is included.
6. Run `pwd`, then `cd ..`, then `pwd` if builtin `cd` is included.
7. Run `export`, `unset`, and `env` with mandatory-only supported forms if included.
8. Run `exit` from an interactive prompt and confirm clean shutdown if included.

## Definition of Done

Sprint 2 is done when:

- MSH-12 is implemented, reviewed, and passes manual bash comparison for mandatory expansion cases.
- MSH-14 is implemented or explicitly remains next if Sprint 2 scope is reduced.
- Existing MSH-11 and MSH-13 behavior has not regressed.
- `make fclean && make` passes.
- Shell-owned allocations are clean on tested paths.
- PRs follow the rule: one parent Jira ticket equals one branch and one PR; subtasks are commits inside the parent PR.

