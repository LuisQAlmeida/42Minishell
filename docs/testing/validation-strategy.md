# Validation Strategy

This document defines the maintained validation strategy for Minishell.

The goal is not to imply test infrastructure that the repository does not yet
have.

Instead, the strategy identifies which checks are currently available, when
they should be used and what evidence is required before making a validation
claim.

## Principles

Validation should be:

- proportional to the change;
- reproducible where practical;
- explicit about what was and was not checked;
- separated from historical project evidence;
- focused on observable behaviour and repository invariants;
- expanded through automation only when the automation actually exists.

A green build is evidence of successful compilation.

It is not evidence that every shell behaviour, signal path, memory lifetime or
file-descriptor transition is correct.

## Validation Layers

The maintained model uses several validation layers.

### 1. Repository and Static Validation

These checks inspect repository artefacts rather than shell runtime behaviour.

Examples include:

```bash
git status
git diff --check
git diff
```

Depending on the change, this layer may also include:

- Markdown link validation;
- YAML or configuration parsing;
- platform-specific schema validation;
- rendered GitHub configuration checks;
- Norminette for source files where 42-style constraints remain applicable.

The exact check should match the artefact being changed.

### 2. Build Validation

The primary local build check is:

```bash
make
```

A broader clean-build check may use:

```bash
make fclean
make
```

Build validation confirms that the project compiles with the repository's
configured compiler flags and dependencies.

The current Makefile does not define a `test` target.

### 3. Behavioural Validation

Shell behaviour currently requires manual validation.

Relevant areas include:

- prompt continuity;
- quoting;
- variable expansion;
- `$?`;
- builtins;
- external command execution;
- PATH resolution;
- redirections;
- heredocs;
- pipelines;
- syntax errors;
- command failures;
- signals;
- shell-state persistence.

The maintained strategy does not describe these behaviours as automatically
covered.

Representative manual checks should be selected according to the subsystem
changed and recorded as validation evidence.

Historical test matrices may be consulted for useful scenarios, but executing
a current check is distinct from citing a historical result.

### 4. Resource Validation

Changes affecting execution, pipelines, redirections, heredocs, builtins or
state ownership may require resource validation.

Relevant concerns include:

- shell-owned memory;
- file-descriptor lifetime;
- descriptor closure across pipelines;
- temporary parent redirections;
- heredoc descriptors;
- child-process cleanup.

Valgrind remains a useful manual tool for these checks.

A representative invocation is:

```bash
valgrind \
  --leak-check=full \
  --show-leak-kinds=all \
  --track-fds=yes \
  ./minishell
```

Results must be interpreted in context.

Historical validation records note that reachable allocations originating from
Readline or related libraries may appear separately from shell-owned memory.

A current validation claim should distinguish library-owned reachable memory
from leaks introduced by Minishell.

For the maintained ownership model, see:

- [`../architecture/resource-ownership.md`](../architecture/resource-ownership.md)
- [`../architecture/process-and-signals.md`](../architecture/process-and-signals.md)

### 5. Automated Regression Validation

A maintained automated regression suite is not currently implemented.

There is currently:

- no repository `tests/` directory;
- no executable Minishell test script;
- no Makefile `test` target;
- no automated behavioural regression job;
- no automated Valgrind or file-descriptor job;
- no coverage reporting.

These are testing gaps, not implicit capabilities.

Automation can be introduced through later dedicated workstreams without
rewriting historical evidence as if it were executable coverage.

## Validation by Change Type

The expected depth of validation depends on the change.

| Change type | Expected validation |
| --- | --- |
| Documentation only | Diff checks, links and relevant rendered/document structure checks |
| GitHub configuration | Syntax/schema validation plus platform behaviour where applicable |
| Build or dependency configuration | Clean build and relevant CI execution |
| Localized source change | Build plus focused behavioural regression |
| Parser / expansion change | Build plus relevant syntax, quote, expansion and status cases |
| Builtin / shell-state change | Build plus state persistence, error status and relevant redirection cases |
| Execution / pipeline change | Build plus pipelines, status propagation and resource checks |
| Redirection / heredoc change | Build plus descriptor, error-path, heredoc and resource checks |
| Signal-sensitive change | Build plus interactive and foreground-process signal checks |
| Resource-ownership change | Build plus representative Valgrind and FD validation |
| CI-only change | Workflow/config validation and observation of the resulting GitHub check |

These are maintained expectations, not claims that every row is automated.

## Current CI Capability

The current GitHub Actions workflow is:

[`../../.github/workflows/ci.yml`](../../.github/workflows/ci.yml)

The detailed CI contract is documented in:

[`../development/continuous-integration.md`](../development/continuous-integration.md)

It runs for:

- pull requests;
- pushes to `main`.

The current `CI / build` job:

1. checks out the repository;
2. installs `libreadline-dev`;
3. performs `make fclean && make`;
4. verifies that `./minishell` is executable;
5. verifies that an unchanged second `make` does not relink the executable.

The current `CI / quality` job:

1. checks out the repository;
2. installs Clang and `libreadline-dev`;
3. performs `make fclean && make CC=clang`;
4. verifies that `./minishell` is executable;
5. verifies that an unchanged second `make CC=clang` does not relink the
   executable.

The workflow also defines explicit read-only repository permissions,
10-minute job timeouts and cancellation of superseded pull-request runs.

The current CI therefore provides both reference build integration and
compiler-diversity validation.

It does not currently run:

- behavioural regression tests;
- Norminette;
- Valgrind;
- file-descriptor checks;
- signal-interaction tests;
- GCC `-fanalyzer`;
- the Clang Static Analyzer;
- `cppcheck`;
- `clang-tidy`;
- coverage reporting.

The compiler-quality and analyzer evaluation is documented in:

[`../development/static-analysis.md`](../development/static-analysis.md)

These limitations should remain visible until the corresponding automation is
actually implemented.

## Historical Evidence

Historical validation lives under:

[`../history/validation/`](../history/validation/)

The preserved material has different purposes.

### Mandatory Test Matrix

[`mandatory-test-matrix.md`](../history/validation/mandatory-test-matrix.md)
contains broad manual evaluation preparation covering the mandatory project
scope, including prompt behaviour, quoting, operators, expansion, builtins,
external execution, redirections, pipelines, signals and error statuses.

It is the broadest historical behavioural reference.

### Expansion Validation

[`MSH-12-expansion-tests.md`](../history/validation/MSH-12-expansion-tests.md)
records focused project-era validation of:

- environment expansion;
- quote-sensitive expansion;
- missing variables;
- `$?`;
- syntax continuity;
- a basic Valgrind pass.

It is historical evidence, not a current automated expansion suite.

### Builtin Validation

[`MSH-14-builtin-tests.md`](../history/validation/MSH-14-builtin-tests.md)
records extensive project-era validation of builtins together with build,
Norminette, error-status, redirection, Valgrind and file-descriptor checks.

Its recorded PASS results describe the historical validation event.

They must not be presented as automatically revalidated on the current
baseline.

## Recording Validation Evidence

A useful validation record answers three questions:

1. What was checked?
2. How was it checked?
3. What was observed?

For example:

```text
Validation:
- make
- manual pipeline regression:
  printf 'a\nb\n' | wc -l
- Valgrind:
  --leak-check=full --show-leak-kinds=all --track-fds=yes
```

Avoid statements such as:

```text
All tests pass.
```

when no defined automated test suite was executed.

Prefer precise claims such as:

```text
The CI build passed and the affected pipeline scenarios were manually
validated.
```

## Known Gaps and Future Automation

The current repository still lacks automated regression coverage.

Future testing work may consider:

- executable behavioural regression tests;
- reusable fixtures and expected-output comparisons;
- non-interactive cases where shell semantics allow reliable automation;
- dedicated interactive testing where terminal behaviour matters;
- CI integration for maintained regression tests;
- automated resource checks where results can be interpreted reliably;
- coverage reporting where it provides useful engineering information.

Compiler-diversity quality validation is now automated through
`CI / quality`.

General static-analysis gates remain separate from compiler diagnostics and
should only be introduced where the analyzer provides demonstrated,
maintainable signal.

Testing automation should be added as executable infrastructure, not inferred
from historical Markdown test matrices.

## Relationship to the Development Workflow

Validation is part of the maintained contribution lifecycle documented in:

[`../development/git-workflow.md`](../development/git-workflow.md)

The pull request should report validation actually performed.

Documentation-only work should not claim runtime testing that was not needed or
performed, while behaviour-changing source work should provide evidence
proportional to its risk.
