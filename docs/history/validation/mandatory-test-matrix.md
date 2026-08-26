# Mandatory Test Matrix

## Purpose

This test matrix is for 42 Minishell mandatory evaluation prep.
It covers only subject-required behavior.
If a mandatory detail is ambiguous, compare with bash and match that result.

## Test Rules

- Run tests in interactive mode unless the test explicitly checks non-interactive behavior.
- Do not include bonus syntax (`&&`, `||`, `()`, `*`) in the mandatory demo.
- Heredoc input must not be added to history.
- After each failure case, verify both the visible error behavior and the resulting `$?`.

## Prompt and History

### PH-01 Prompt appears

- **Command:** start `./minishell`
- **Expected outcome:** a prompt is displayed and the shell waits for input.
- **Expected `$?`:** not applicable until a command runs.

### PH-02 History stores normal commands

- **Commands:**
  - `echo hello`
  - `history` is not a required builtin, so verify manually with up-arrow in `readline`
- **Expected outcome:** previous command can be recalled via interactive history navigation.
- **Expected `$?`:** `0` after `echo hello`.

### PH-03 Empty line does not break loop

- **Command:** press `Enter` on an empty prompt
- **Expected outcome:** shell prints a new prompt and stays interactive.
- **Expected `$?`:** unchanged from the previous command.

### PH-04 Heredoc body is excluded from history

- **Commands:**
  - `cat << EOF`
  - `line one`
  - `line two`
  - `EOF`
- **Expected outcome:** heredoc works and `cat` prints the entered lines.
- **Expected history outcome:** the command line may be in history, but heredoc body lines must not be added.
- **Expected `$?`:** `0` if heredoc completes successfully.

## Quotes and Unclosed Quotes

### Q-01 Single quotes preserve literal text

- **Command:** `echo '$HOME | > < ?'`
- **Expected outcome:** output is literal: `$HOME | > < ?`
- **Expected `$?`:** `0`

### Q-02 Double quotes allow variable expansion

- **Command:** `echo "home=$HOME"`
- **Expected outcome:** `$HOME` is expanded inside the double-quoted string.
- **Expected `$?`:** `0`

### Q-03 Adjacent quote segments join into one argument

- **Command:** `echo 'mini'"shell"`
- **Expected outcome:** output is `minishell`
- **Expected `$?`:** `0`

### Q-04 Unclosed single quote is rejected

- **Command:** `echo 'unterminated`
- **Expected outcome:** command is not executed; shell reports a parse/input error and returns to prompt.
- **Expected `$?`:** non-zero

### Q-05 Unclosed double quote is rejected

- **Command:** `echo "unterminated`
- **Expected outcome:** command is not executed; shell reports a parse/input error and returns to prompt.
- **Expected `$?`:** non-zero

## Operators: `|`, `<`, `>`, `<<`, `>>`

### OP-01 Single pipe

- **Command:** `echo hello | cat`
- **Expected outcome:** `cat` receives `hello` from the pipe.
- **Expected `$?`:** `0`

### OP-02 Input redirection

- **Setup command:** `printf 'abc\n' > in.txt`
- **Test command:** `cat < in.txt`
- **Expected outcome:** output is `abc`
- **Expected `$?`:** `0`

### OP-03 Output redirection truncate

- **Command:** `echo first > out.txt`
- **Expected outcome:** `out.txt` contains `first` and is truncated before writing.
- **Expected `$?`:** `0`

### OP-04 Output redirection append

- **Commands:**
  - `echo first > out.txt`
  - `echo second >> out.txt`
- **Expected outcome:** `out.txt` contains both lines, with the second appended.
- **Expected `$?`:** `0` after each successful command.

### OP-05 Heredoc basic flow

- **Commands:**
  - `cat << EOF`
  - `abc`
  - `EOF`
- **Expected outcome:** `cat` prints `abc`
- **Expected `$?`:** `0`

### OP-06 Missing right-hand operand after redirection

- **Command:** `echo hello >`
- **Expected outcome:** syntax error, no command execution.
- **Expected `$?`:** non-zero

### OP-07 Missing command after pipe

- **Command:** `echo hello |`
- **Expected outcome:** syntax error, no pipeline execution.
- **Expected `$?`:** non-zero

## Environment Expansion: `$VAR` and `$?`

### EX-01 Expand existing variable

- **Command:** `echo $HOME`
- **Expected outcome:** prints the current `HOME` value.
- **Expected `$?`:** `0`

### EX-02 Expand missing variable

- **Command:** `echo "X$THIS_DOES_NOT_EXISTY"`
- **Expected outcome:** missing variable expands to empty string, so output is `X`
- **Expected `$?`:** `0`

### EX-03 No expansion in single quotes

- **Command:** `echo '$HOME $?'`
- **Expected outcome:** output stays literal.
- **Expected `$?`:** `0`

### EX-04 `$?` after success

- **Commands:**
  - `echo ok`
  - `echo $?`
- **Expected outcome:** second command prints `0`
- **Expected `$?`:** `0`

### EX-05 `$?` after command-not-found

- **Commands:**
  - `this_command_should_not_exist`
  - `echo $?`
- **Expected outcome:** first command reports failure; second command prints a non-zero status.
- **Expected `$?`:** first command non-zero, second command `0`

### EX-06 `$?` after failing builtin or syscall-backed error

- **Commands:**
  - `cd /path/that/does/not/exist`
  - `echo $?`
- **Expected outcome:** `cd` reports an error; second command prints a non-zero status.
- **Expected `$?`:** first command non-zero, second command `0`

## Builtins (Subject-Scoped Only)

### BI-01 `echo`

- **Commands:**
  - `echo hello`
  - `echo -n hello`
- **Expected outcome:** first prints `hello` plus newline; second prints `hello` without trailing newline.
- **Expected `$?`:** `0`

### BI-02 `pwd`

- **Command:** `pwd`
- **Expected outcome:** prints current working directory.
- **Expected `$?`:** `0`

### BI-03 `cd` with absolute path

- **Commands:**
  - `cd /tmp`
  - `pwd`
- **Expected outcome:** working directory changes to `/tmp`
- **Expected `$?`:** `0` if `/tmp` is accessible

### BI-04 `cd` with relative path

- **Commands:**
  - `mkdir -p testdir`
  - `cd testdir`
  - `pwd`
- **Expected outcome:** shell enters `testdir`
- **Expected `$?`:** `0`

### BI-05 `export`

- **Commands:**
  - `export TESTVAR=42`
  - `echo $TESTVAR`
- **Expected outcome:** variable is stored in shell environment and expands to `42`
- **Expected `$?`:** `0`

### BI-06 `unset`

- **Commands:**
  - `export TESTVAR=42`
  - `unset TESTVAR`
  - `echo $TESTVAR`
- **Expected outcome:** variable no longer expands.
- **Expected `$?`:** `0`

### BI-07 `env`

- **Command:** `env`
- **Expected outcome:** prints environment entries; exported variables appear.
- **Expected `$?`:** `0`

### BI-08 `exit`

- **Command:** `exit`
- **Expected outcome:** shell terminates cleanly.
- **Expected `$?`:** shell process exits successfully for plain `exit`.

### BI-09 Unsupported extra builtin options are out of scope

- **Command:** `pwd -P`
- **Expected outcome:** behavior should stay simple and subject-scoped; do not claim support for extra options not required by the subject.
- **Expected `$?`:** implementation-defined within subject scope, but it must not crash.

## External Execution and PATH Resolution

### EE-01 Resolve through `PATH`

- **Command:** `ls`
- **Expected outcome:** command is found through `PATH` and executes.
- **Expected `$?`:** `0` on success

### EE-02 Relative path execution

- **Setup command:** compile or create an executable script/binary in current directory
- **Test command:** `./myprog`
- **Expected outcome:** executable runs via relative path.
- **Expected `$?`:** command-defined, usually `0` on success

### EE-03 Absolute path execution

- **Command:** `/bin/echo hello`
- **Expected outcome:** executable runs via absolute path and prints `hello`
- **Expected `$?`:** `0`

### EE-04 Command not found

- **Command:** `definitely_not_a_real_command`
- **Expected outcome:** error message is shown; shell stays alive.
- **Expected `$?`:** non-zero

### EE-05 Non-executable target

- **Setup command:** `echo text > file.txt`
- **Test command:** `./file.txt`
- **Expected outcome:** execution fails with an error; shell stays alive.
- **Expected `$?`:** non-zero

## Redirections

### RD-01 Input redirection from existing file

- **Setup command:** `printf 'line\n' > input.txt`
- **Test command:** `cat < input.txt`
- **Expected outcome:** output is `line`
- **Expected `$?`:** `0`

### RD-02 Input redirection missing file

- **Command:** `cat < no_such_file`
- **Expected outcome:** open error is reported; command does not read input.
- **Expected `$?`:** non-zero

### RD-03 Output redirection creates file

- **Command:** `echo hello > created.txt`
- **Expected outcome:** file is created and contains `hello`
- **Expected `$?`:** `0`

### RD-04 Append redirection

- **Commands:**
  - `echo a > append.txt`
  - `echo b >> append.txt`
- **Expected outcome:** file contains both `a` and `b` in order.
- **Expected `$?`:** `0`

### RD-05 Heredoc with delimiter

- **Commands:**
  - `cat << END`
  - `one`
  - `two`
  - `END`
- **Expected outcome:** command reads until the delimiter line and prints `one` and `two`.
- **Expected `$?`:** `0`

### RD-06 Multiple output redirections, last one wins

- **Command:** `echo hello > a.txt > b.txt`
- **Expected outcome:** final stdout target is `b.txt`; the effective output goes there.
- **Expected `$?`:** `0`

### RD-07 Multiple input redirections, last one wins

- **Commands:**
  - `printf 'A\n' > in1.txt`
  - `printf 'B\n' > in2.txt`
  - `cat < in1.txt < in2.txt`
- **Expected outcome:** effective stdin is from `in2.txt`, so output is `B`
- **Expected `$?`:** `0`

### RD-08 Builtin with redirection in parent

- **Commands:**
  - `pwd > pwd.txt`
  - `cat pwd.txt`
- **Expected outcome:** builtin output is redirected correctly and parent stdio is restored for the next prompt.
- **Expected `$?`:** `0`

## Pipelines

### PL-01 Two-stage pipeline

- **Command:** `echo hello | wc -c`
- **Expected outcome:** second command receives the first command output through the pipe.
- **Expected `$?`:** `0` if the pipeline succeeds

### PL-02 N-stage pipeline

- **Command:** `printf 'a\nb\nc\n' | cat | wc -l`
- **Expected outcome:** output is `3`
- **Expected `$?`:** `0`

### PL-03 Pipeline with input redirection

- **Commands:**
  - `printf 'x\ny\n' > pipe_in.txt`
  - `cat < pipe_in.txt | wc -l`
- **Expected outcome:** output is `2`
- **Expected `$?`:** `0`

### PL-04 Pipeline with output redirection on last stage

- **Command:** `echo hello | cat > pipe_out.txt`
- **Expected outcome:** `pipe_out.txt` receives pipeline output.
- **Expected `$?`:** `0`

### PL-05 Builtin inside pipeline

- **Command:** `echo hello | cat`
- **Expected outcome:** builtin `echo` works as a pipeline producer.
- **Expected `$?`:** `0`

### PL-06 Stateful builtin inside pipeline does not mutate parent shell

- **Commands:**
  - `pwd`
  - `cd /tmp | cat`
  - `pwd`
- **Expected outcome:** the parent shell working directory does not change because the builtin runs in pipeline context.
- **Expected `$?`:** pipeline result is set; shell remains usable

## Signals

### SG-01 `Ctrl-C` at prompt

- **Action:** press `Ctrl-C` on an empty prompt
- **Expected outcome:** a newline is printed and a fresh prompt appears.
- **Expected `$?`:** shell sets a signal-related non-zero status consistent with bash-like behavior for interrupted interactive input.

### SG-02 `Ctrl-D` at prompt

- **Action:** press `Ctrl-D` on an empty prompt
- **Expected outcome:** shell exits cleanly.
- **Expected `$?`:** shell terminates without crashing.

### SG-03 `Ctrl-\` at prompt

- **Action:** press `Ctrl-\` on an empty prompt
- **Expected outcome:** nothing visible happens; shell remains at prompt.
- **Expected `$?`:** unchanged

### SG-04 `Ctrl-C` during a foreground command

- **Command:** `cat`, then press `Ctrl-C`
- **Expected outcome:** foreground command is interrupted; shell recovers and returns to prompt.
- **Expected `$?`:** non-zero signal-related status

## Error Cases and Expected `$?` Behavior

Use these as quick spot-checks during evaluation.

### ER-01 Syntax error

- **Command:** `|`
- **Expected outcome:** parser rejects input; no execution.
- **Expected `$?`:** non-zero

### ER-02 Redirection open failure

- **Command:** `cat < does_not_exist`
- **Expected outcome:** file open error.
- **Expected `$?`:** non-zero

### ER-03 Command not found

- **Command:** `notfound`
- **Expected outcome:** resolution failure is reported.
- **Expected `$?`:** non-zero

### ER-04 Failing builtin

- **Command:** `cd /definitely/not/here`
- **Expected outcome:** builtin reports an error and shell stays interactive.
- **Expected `$?`:** non-zero

### ER-05 `$?` reflects the latest foreground pipeline

- **Commands:**
  - `notfound`
  - `echo $?`
  - `echo ok`
  - `echo $?`
- **Expected outcome:** the first `echo $?` prints a non-zero value; the second prints `0`.
- **Expected `$?`:** each `echo` itself returns `0`

## Suggested Demo Sequence

Use a short sequence that touches every mandatory subsystem without wasting evaluator time.

1. `echo "hello $HOME"`
2. `echo 'literal $HOME'`
3. `export TESTVAR=42`
4. `echo $TESTVAR`
5. `printf 'a\nb\n' > demo.txt`
6. `cat < demo.txt | wc -l`
7. `echo first > out.txt`
8. `cat << EOF` then enter one line and close with `EOF`
9. `cd /tmp`
10. `pwd`
11. `definitely_not_a_command`
12. `echo $?`
13. `Ctrl-C` at empty prompt

## Pre-Eval Checklist

- Norm check passes for mandatory files.
- No leaks in our code paths under normal command execution.
- Readline-internal leaks are understood and separated from our own memory ownership.
- No fd leaks after redirections, heredocs, and N-stage pipelines.
- Builtins that must mutate shell state run in the parent when standalone.
- Builtins in pipelines do not incorrectly mutate the parent shell.
- `$?` is updated after success, failure, and signal interruption.
- Unclosed quotes and malformed operators fail cleanly without crashes.
- Demo script is rehearsed and short enough to run under evaluator time pressure.
- Team can explain:
  - parser model
  - fd lifecycle
  - signal strategy
  - memory cleanup ownership
