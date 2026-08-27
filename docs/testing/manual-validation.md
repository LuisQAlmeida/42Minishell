# Manual Validation

This guide provides maintained, reproducible manual checks for the current
Minishell implementation.

It complements
[`validation-strategy.md`](validation-strategy.md).

The commands below are validation procedures, not claims that every check has
already been executed against every commit.

Historical project-era results remain preserved separately under
[`../history/validation/`](../history/validation/).

## How to Use This Guide

Choose validation proportional to the change.

A documentation-only pull request does not require an interactive Minishell
regression run merely to satisfy a checklist.

A source change should validate the affected behaviour together with nearby
regression risks.

For each validation session, record:

- the build or commit being tested;
- the commands or interaction performed;
- the observed result;
- any relevant exit status;
- failures or deviations;
- resource-tool output where applicable.

## 1. Clean Build

From the repository root:

```bash
make fclean
make
```

Expected result:

- compilation completes successfully;
- the `minishell` executable is produced;
- compiler flags remain `-Wall -Wextra -Werror`.

A second build may be used to check that an unchanged tree does not rebuild
unnecessarily:

```bash
make
```

The current Makefile does not provide a `test` target.

## 2. Start the Shell

Run:

```bash
./minishell
```

Confirm that:

- the prompt appears;
- an empty input line does not terminate the shell;
- normal commands return control to the prompt;
- the shell remains usable after recoverable errors.

Exit with:

```text
exit
```

## 3. Basic Command Execution

Inside Minishell:

```text
echo hello
pwd
/bin/echo absolute
echo external
```

Check that:

- builtins execute successfully;
- absolute executable paths work;
- commands resolvable through `PATH` execute;
- control returns to the prompt.

For an unknown command:

```text
definitely_not_a_command
echo $?
```

The maintained implementation should report command-not-found behaviour and
store the corresponding shell status.

The historical mandatory validation expected status `127` for this case.

## 4. Quotes and Expansion

Use representative quote and expansion cases:

```text
echo '$HOME'
echo "$HOME"
echo $HOME
echo "$THIS_VARIABLE_SHOULD_NOT_EXIST"
```

Check that:

- single quotes preserve `$HOME` literally;
- double quotes allow expansion;
- unquoted variables expand;
- a missing variable expands according to the maintained implementation.

Check the previous exit status:

```text
pwd
echo $?
definitely_not_a_command
echo $?
```

When changing scanning or expansion code, also include concatenated segments
such as:

```text
echo "prefix"$HOME
echo 'literal'"$HOME"
```

## 5. Syntax Error Recovery

Try malformed input such as:

```text
echo "unterminated
```

and malformed operators relevant to the implementation, for example:

```text
echo hello |
```

Confirm that:

- the shell reports the error;
- it does not crash;
- it returns to a usable prompt;
- subsequent commands still execute.

Record the resulting `$?` when the change affects syntax-status behaviour.

## 6. Builtins and Persistent Shell State

Representative standalone builtin checks include:

```text
pwd
cd /tmp
pwd
cd /does/not/exist
echo $?
```

Environment mutation may be checked with:

```text
export MINISHELL_VALIDATION=hello
echo $MINISHELL_VALIDATION
export MINISHELL_VALIDATION=world
echo $MINISHELL_VALIDATION
unset MINISHELL_VALIDATION
echo "$MINISHELL_VALIDATION"
```

Check that standalone stateful builtins mutate the parent shell as expected.

When relevant, also verify that a stateful builtin executed inside a pipeline
does not incorrectly mutate persistent parent-shell state.

## 7. Redirections

Create disposable fixtures before or during validation.

Representative checks include:

```text
echo first > validation-output.txt
cat validation-output.txt
echo second >> validation-output.txt
cat validation-output.txt
cat < validation-output.txt
```

Check:

- truncate output redirection;
- append redirection;
- input redirection;
- creation and reuse of files;
- return to normal terminal input/output afterward.

For redirection failure:

```text
cat < file-that-does-not-exist
echo $?
```

Confirm that the shell remains usable after the error.

Remove validation artefacts afterward:

```text
rm -f validation-output.txt
```

## 8. Pipelines

Representative pipeline checks include:

```text
printf 'a\nb\n' | wc -l
echo hello | cat
printf 'a\nb\nc\n' | grep b | wc -l
```

Check:

- two-stage pipelines;
- multi-stage pipelines;
- data flow between stages;
- shell continuity afterward.

When pipeline status behaviour is relevant, execute a pipeline and inspect:

```text
echo $?
```

The maintained implementation uses the foreground pipeline result for the
shell's persistent last status.

## 9. Redirections with Pipelines

Combine the two mechanisms when execution or descriptor ownership changes:

```text
printf 'a\nb\n' > validation-input.txt
cat < validation-input.txt | wc -l
cat validation-input.txt | grep b > validation-output.txt
cat validation-output.txt
```

Confirm that:

- input redirection reaches the intended command;
- pipeline descriptors are connected correctly;
- output redirection applies to the intended stage;
- later prompt iterations still use normal standard input and output.

Clean up:

```text
rm -f validation-input.txt validation-output.txt
```

## 10. Heredoc

Run:

```text
cat << EOF
line one
line two
EOF
```

Confirm that:

- input is collected until the delimiter;
- the resulting content reaches the command;
- the prompt returns afterward.

When expansion behaviour is under test, check an unquoted delimiter using a
known environment variable.

When quoted-delimiter semantics are relevant to the change, validate them
against the maintained implementation and record the observed result.

## 11. Interactive Signals

Signal validation must be performed in an interactive terminal.

### Prompt context

At an empty Minishell prompt:

- press `Ctrl-C`;
- confirm that the shell remains running;
- confirm that a fresh prompt is presented.

Test `Ctrl-\` separately and record the observed prompt behaviour.

Use `Ctrl-D` at an empty prompt to verify end-of-input handling.

### Foreground command

Run a foreground process such as:

```text
cat
```

Then press `Ctrl-C`.

Confirm that:

- the foreground process is interrupted;
- Minishell itself remains running;
- the prompt returns;
- `$?` reflects signal-derived termination according to the maintained process
  model.

Signal-derived process status is documented in:

[`../architecture/process-and-signals.md`](../architecture/process-and-signals.md)

### Heredoc context

When heredoc signal handling is affected, start:

```text
cat << EOF
```

and press `Ctrl-C` before supplying the delimiter.

Confirm that:

- heredoc collection is interrupted;
- the shell returns to its interactive state;
- subsequent commands remain usable.

## 12. Memory Validation

For changes that affect allocation ownership or long-lived shell state, run:

```bash
valgrind \
  --leak-check=full \
  --show-leak-kinds=all \
  ./minishell
```

Exercise representative affected paths before exiting.

Inspect separately:

- definitely lost memory;
- indirectly lost memory;
- possibly lost memory;
- still-reachable allocations;
- Valgrind error summaries.

Do not classify every reachable allocation as a Minishell leak.

Historical validation identified reachable allocations associated with
Readline and related libraries. Current results still need to be interpreted
against actual ownership rather than copied from historical summaries.

See:

[`../architecture/resource-ownership.md`](../architecture/resource-ownership.md)

## 13. File-Descriptor Validation

For execution, pipelines, heredocs and redirections, include descriptor
tracking:

```bash
valgrind \
  --leak-check=full \
  --show-leak-kinds=all \
  --track-fds=yes \
  ./minishell
```

Exercise representative scenarios such as:

```text
echo hello | cat
printf 'a\nb\n' | grep b | wc -l
echo hello > validation-output.txt
cat < validation-output.txt
cat << EOF
heredoc
EOF
```

After leaving Minishell, inspect Valgrind's descriptor report.

The relevant question is not simply whether descriptors existed during
execution, but whether shell-owned descriptors that should have been released
remain open at exit.

Clean up any generated files afterward.

## 14. Validation After an Error

Error paths deserve explicit regression checks because they often exercise
different cleanup paths.

Representative cases include:

```text
definitely_not_a_command
cat < file-that-does-not-exist
cd /does/not/exist
echo hello |
```

After each case:

1. observe the error;
2. inspect `$?` when relevant;
3. run a normal command such as `echo recovered`;
4. confirm that Minishell remains usable.

For resource-sensitive changes, include failure paths in Valgrind and FD
validation rather than testing only successful commands.

## 15. Documentation and Repository Changes

Changes that do not affect runtime behaviour should use validation appropriate
to the artefact.

Typical checks include:

```bash
git status
git diff --check
git diff
```

Additional checks may include:

- Markdown link validation;
- YAML parsing;
- GitHub-specific configuration validation;
- rendered GitHub behaviour;
- documentation structure audits.

Do not claim runtime testing if runtime behaviour was not changed or exercised.

## 16. Recording Results in a Pull Request

Report only validation actually performed.

A useful record looks like:

```text
Validation:
- make fclean && make
- manual expansion regression:
  - echo '$HOME'
  - echo "$HOME"
  - invalid command followed by echo $?
- Valgrind with --track-fds=yes on representative redirection paths
```

For documentation-only work:

```text
Validation:
- git diff --check
- local Markdown link audit
```

Avoid vague claims such as:

```text
All tests pass.
```

unless a defined test suite was actually executed.

## Historical Scenario Reference

The historical validation documents remain useful sources of scenarios:

- [`mandatory-test-matrix.md`](../history/validation/mandatory-test-matrix.md)
  provides broad mandatory-subsystem coverage;
- [`MSH-12-expansion-tests.md`](../history/validation/MSH-12-expansion-tests.md)
  provides focused expansion scenarios;
- [`MSH-14-builtin-tests.md`](../history/validation/MSH-14-builtin-tests.md)
  provides extensive builtin, status and resource scenarios.

Using a historical scenario as a current manual check is valid.

A historical PASS result by itself is not evidence that the current baseline
was revalidated.
