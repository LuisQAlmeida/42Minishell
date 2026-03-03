# Sprint 1

## Sprint Goal

Deliver the first mandatory vertical slice of Minishell:

- prompt loop + baseline signals
- tokenization basics + quote handling
- parse single command
- execute a single external command via `PATH`
- minimal cleanup with no leaks in these code paths

This sprint is intentionally narrow.
It does not include pipelines, redirections, builtins beyond what is needed to support the shell loop, or variable expansion.

## Selected Stories

Sprint 1 uses narrow slices from the mandatory backlog.

### M1 (selected scope): Interactive Shell Bootstrap and Signal Baseline

Included this sprint:

- prompt display
- `readline` loop
- add non-empty commands to history
- `Ctrl-C` prompt refresh behavior
- `Ctrl-D` exits the shell
- `Ctrl-\` does nothing at the prompt
- one global signal number only

Excluded this sprint:

- signal behavior during complex execution flows not yet implemented

### M2 (selected scope): Mandatory Tokenization and Syntax Validation

Included this sprint:

- tokenize a single command line into words
- recognize single quotes and double quotes
- reject unclosed quotes
- keep unsupported operators out of scope

Excluded this sprint:

- pipe token handling
- redirection token handling
- multi-command syntax validation

### M4 (selected scope): External Command Resolution and Single Command Execution

Included this sprint:

- resolve executable through `PATH`
- support absolute and relative executable paths
- `fork` + `execve` for one command
- wait for foreground child
- store exit status internally for the shell loop

Excluded this sprint:

- pipelines
- redirections
- builtin execution routing

### Cleanup Slice (from M8 intent, not full story)

Included this sprint:

- free shell-owned memory for:
  - input line
  - token list
  - parsed single-command structure
  - temporary `envp`/path-resolution buffers
- verify no memory leaks on the implemented paths

Excluded this sprint:

- full mandatory regression matrix
- README and final release hardening

## Planned Deliverables

- Interactive `./minishell` starts and stays in a stable prompt loop.
- Single-command input with quoted arguments is tokenized correctly.
- Unclosed quotes are rejected without crashing.
- A parsed single command can execute external programs such as `ls`, `/bin/echo`, and `./local_binary`.
- Shell survives command failures and returns to the next prompt.
- Implemented paths are clean under leak checks for shell-owned allocations.

## Task Split

The split favors parallel work with one integration seam: `tokenize -> parse single command -> execute`.

### Luís

Primary ownership:

- prompt loop integration around `readline`
- baseline signal setup and signal state handling
- shell state struct for:
  - last status
  - current input line
  - global loop control
- main cleanup path for one loop iteration

Concrete tasks:

- Create main loop entry point and prompt rendering.
- Add history only for non-empty lines.
- Implement signal handlers with one global signal number.
- Handle `Ctrl-C`, `Ctrl-D`, and `Ctrl-\` in interactive prompt mode.
- Build loop-level cleanup for input line and iteration-owned objects.
- Integrate parser/executor calls once João's interfaces are ready.

### João

Primary ownership:

- tokenizer for single-command mandatory subset
- quote handling
- single-command parser output
- single external command execution and `PATH` resolution

Concrete tasks:

- Implement tokenization of words with single and double quote handling.
- Detect and reject unclosed quotes before execution.
- Convert token stream into a single-command parsed structure (`argv` only for this sprint).
- Implement command resolution via `PATH`, relative path, and absolute path.
- Implement `fork`, `execve`, `wait/waitpid`, and exit-status capture.
- Free parser/execution temporary allocations in success and failure paths.

### Shared Integration

- Agree on the handoff interface before coding:
  - tokenizer output type
  - parse output type
  - executor entry point
  - error/status return conventions
- Pair on final integration tests for:
  - quoted arguments
  - command-not-found
  - signal recovery at prompt
  - leak-check runs

## Integration Order

1. Luís lands prompt loop skeleton and signal baseline.
2. João lands tokenizer with quote validation.
3. João lands single-command parser and executor entry point.
4. Luís integrates parser/executor into the interactive loop.
5. Both run cleanup and leak checks on implemented flows.

## Risks and Mitigations

### Risk 1: Interface mismatch between tokenizer, parser, and executor

- **Impact:** parallel work stalls during merge.
- **Mitigation:** define shared structs and function signatures before implementation starts; do a same-day integration checkpoint.

### Risk 2: Signal handling interferes with `readline`

- **Impact:** prompt duplication, broken line state, or unstable interactive behavior.
- **Mitigation:** keep the signal handler minimal, store only the signal number globally, and handle prompt refresh in the main loop after `readline`-safe points.

### Risk 3: Quote handling grows into parser complexity too early

- **Impact:** overengineering and missed sprint scope.
- **Mitigation:** limit parsing to one command with `argv`; treat quotes as tokenization concerns for this sprint; defer operators to later stories.

### Risk 4: Memory leaks in error paths

- **Impact:** early technical debt and unstable foundations.
- **Mitigation:** require one cleanup function per stage, test both success and failure cases, and verify command-not-found plus unclosed-quote paths under leak tooling.

### Risk 5: PATH resolution edge cases consume too much time

- **Impact:** execution work slips.
- **Mitigation:** support only the mandatory subset now:
  - direct absolute path
  - direct relative path
  - plain command search through `PATH`
  - clear failure when not found

## Sprint Boundaries

Explicitly not in Sprint 1:

- `|`, `<`, `>`, `<<`, `>>`
- `$VAR` and `$?` expansion
- builtins beyond basic shell loop concerns
- parent-vs-child builtin routing
- heredoc handling
- bonus syntax

If any of these start pulling design complexity into the sprint, stop and defer them.

## Demo Scenarios

Use these as the sprint review demo.

1. Start `./minishell` and show the prompt.
2. Run `ls`.
3. Run `/bin/echo hello`.
4. Run `echo 'hello world'` only if external `/bin/echo` is used explicitly; do not imply builtin support yet.
5. Run `/bin/echo "home:$HOME"` only as a literal-argument quote handling test, not an expansion test.
6. Enter an unclosed quote and show clean rejection without crash.
7. Run a non-existent command and show failure without shell exit.
8. Press `Ctrl-C` at the prompt and show a clean fresh prompt.

## Definition of Done

Sprint 1 is done only when all of the following are true:

- Shell starts in interactive mode and consistently shows a prompt.
- Non-empty successful inputs are added to history.
- `Ctrl-C`, `Ctrl-D`, and `Ctrl-\` behave correctly at the prompt for the implemented scope.
- Tokenizer correctly handles plain words, single quotes, double quotes, and rejects unclosed quotes.
- Parser produces a valid single-command structure for the supported input subset.
- Executor runs one external command via `PATH`, relative path, and absolute path.
- Parent waits for the child and keeps the shell alive after command completion or failure.
- Command-not-found and parse-error paths return control to the prompt without crashes.
- All shell-owned allocations in the implemented paths are released.
- No fd leaks exist in the implemented single-command execution path.
- Both teammates can explain the chosen interfaces and cleanup ownership during review.
