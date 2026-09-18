# Behavioural Regression Tests

This directory contains the maintained non-interactive behavioural regression
suite for Minishell.

## Run

From the repository root:

```bash
make test
```

`make test` builds Minishell when necessary and then runs:

```bash
tests/regression.sh
```

## Scope

The suite covers deterministic mandatory behaviour that can be exercised
reliably with redirected standard input.

Current coverage includes:

- builtin command behaviour;
- external command execution and PATH resolution;
- quoting and environment expansion;
- `$?` and shell exit statuses;
- persistent shell state for standalone stateful builtins;
- syntax-error recovery;
- pipelines and final-stage status;
- input, output and append redirections;
- parent stdio restoration after builtin redirection;
- basic heredoc input;
- explicit `exit` status handling.

The suite uses a controlled environment and a temporary working directory so
tests do not depend on the developer's HOME, PATH contents or repository-local
fixtures.

## Continuous Integration

GitHub Actions executes the same maintained suite through the dedicated
`CI / regression` job on pull requests and pushes to `main`.

The CI job keeps behavioural regression results separate from reference-build,
compiler-diversity and documentation checks so failures remain easy to
interpret.

## Output Normalization

Readline prints the Minishell prompt and echoes redirected input even when the
test harness feeds commands non-interactively.

The harness removes those transport artifacts before comparing functional
stdout.

Heredoc input prompts are normalized in the same way.

This normalization is deliberately narrow: the tests compare the command
output, stderr and process status rather than treating the interactive prompt
transcript as behavioural output.

## What Remains Manual

The initial regression suite does not automate behaviour that depends on a real
interactive terminal, including:

- prompt/history interaction;
- `Ctrl-C`, `Ctrl-D` and `Ctrl-\`;
- foreground-process signal interaction;
- heredoc interruption by signals;
- Readline-specific terminal behaviour.

Memory, file-descriptor, static-analysis and coverage checks are also separate
from this behavioural suite.

See [`../docs/testing/manual-validation.md`](../docs/testing/manual-validation.md)
for maintained manual validation guidance.

Historical project-era test matrices remain under
[`../docs/history/validation/`](../docs/history/validation/) as provenance and
scenario evidence. They are not treated as automatically executed coverage.

## Isolation

Each regression case runs in a temporary working directory created with
`mktemp`.

The suite:

- uses a controlled environment;
- creates fixtures only under that temporary directory;
- removes the temporary tree on exit;
- does not write test artifacts into the repository.

A failing test reports differences in stdout, stderr or process status and
causes the suite to exit non-zero.
