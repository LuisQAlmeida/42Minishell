# Continuous Integration

This document describes the maintained continuous-integration baseline for the
repository.

The current GitHub Actions workflow provides reference build integration and
compiler-diversity quality checks. It remains deliberately narrower than a
complete behavioural, resource-safety or static-analysis gate.

The authoritative workflow configuration is:

[`../../.github/workflows/ci.yml`](../../.github/workflows/ci.yml)

## Purpose

The CI baseline answers a focused question:

> Can the repository be checked out in the maintained CI environment,
> produce the expected Minishell executable through the reference build and
> also compile cleanly through the selected independent compiler check?

Both maintained jobs check one important Makefile invariant: rebuilding an
unchanged tree must not relink the final executable.

The workflow does not currently claim behavioural test coverage or a general
static-analysis guarantee.

## When CI Runs

The workflow is triggered by:

- pull requests;
- pushes to `main`.

An ordinary push to a feature branch does not trigger this workflow until a
pull request exists.

The maintained flow is:

    feature branch push
            |
            v
    no CI run required yet

    pull request opened or updated
            |
            v
    CI / build
    CI / quality

    squash merge to main
            |
            v
    CI / build
    CI / quality

Pull-request runs that are superseded by newer commits are cancelled.

Push runs on `main` are not cancelled by that pull-request policy.

## Execution Environment

Both maintained jobs use:

    ubuntu-24.04

Using an explicit runner version avoids silently following the moving
`ubuntu-latest` alias.

Ubuntu 24.04 is the maintained CI reference environment. It is not a claim
that Minishell can only compile or run on Ubuntu 24.04.

The reference `build` job uses the compiler selected by the repository
Makefile through:

    CC = cc

The `quality` job installs Clang and overrides the Make variable only for that
CI invocation:

    make CC=clang

This does not modify the Makefile or replace the original compiler interface.

## Workflow Permissions

The workflow declares:

```yaml
permissions:
  contents: read
```

The build needs repository contents but does not need write access.

The checkout step also uses:

```yaml
persist-credentials: false
```

because the job does not push commits, create tags or modify repository state.

These settings keep the build workflow read-only at the repository level.

## Build Dependency

The reference build requires the Readline development package:

    libreadline-dev

The quality job requires:

    clang
    libreadline-dev

The workflow installs these dependencies through the Ubuntu package manager
with `--no-install-recommends`.

The project links against Readline through the existing Makefile.

The local `libft` is built from repository source and is not installed as an
external package.

## Build Pipeline

The workflow contains two independent jobs.

### Reference Build

`CI / build`:

1. checks out the repository without persisting credentials;
2. installs `libreadline-dev`;
3. runs `make fclean`;
4. runs the normal `make` reference build;
5. verifies that `./minishell` exists and is executable;
6. runs `make` again and verifies that the executable timestamp does not
   change.

The normal Makefile remains authoritative for the reference build.

### Compiler-Diversity Quality Build

`CI / quality`:

1. checks out the repository without persisting credentials;
2. installs Clang and `libreadline-dev`;
3. runs `make fclean`;
4. runs `make CC=clang`;
5. verifies that `./minishell` exists and is executable;
6. runs `make CC=clang` again and verifies that the executable timestamp does
   not change.

The `CC=clang` value is a command-line Make override.

It does not change the repository's:

    CC = cc

setting.

Both jobs therefore exercise the same source tree and Makefile while using
independent compiler paths.

The second build in each job validates the existing no-relink Makefile
invariant.

## Failure Semantics

A failed `CI / build` check means that at least one reference
build-integration requirement was not satisfied.

Examples include:

- dependency installation failure;
- `make fclean` failure;
- reference compilation failure;
- failure to produce an executable `./minishell`;
- unexpected relinking during the second `make`;
- job timeout;
- another unsuccessful workflow step.

A failed `CI / quality` check means that at least one maintained
compiler-diversity requirement was not satisfied.

Examples include:

- Clang installation failure;
- `make fclean` failure;
- compilation failure under `make CC=clang`;
- a Clang diagnostic promoted to an error by `-Werror`;
- failure to produce an executable `./minishell`;
- unexpected relinking during the second `make CC=clang`;
- job timeout;
- another unsuccessful workflow step.

Neither failed check identifies every possible runtime defect.

Likewise, successful checks do not imply behavioural correctness, resource
safety or general static-analysis cleanliness.

## Concurrency Behaviour

The workflow groups runs using the workflow identity together with the pull
request number or Git reference.

For pull requests:

```text
new commit pushed
       |
       v
new CI run starts
       |
       +--> older in-progress run is cancelled
```

This avoids spending runner time on an obsolete revision of the same pull
request.

For non-pull-request events, including pushes to `main`, the cancellation
condition is false.

## What CI Currently Guarantees

A successful current CI run provides two complementary build signals.

`CI / build` confirms that:

- the repository can perform a clean reference build through `CC = cc`;
- the expected Minishell executable is produced;
- an unchanged second build does not relink the executable.

`CI / quality` confirms that:

- the same repository and Makefile compile cleanly through Clang;
- the existing `-Wall -Wextra -Werror` diagnostics remain clean under that
  compiler;
- the expected Minishell executable is produced;
- an unchanged second Clang build does not relink the executable.

These are build-integration and compiler-diversity guarantees.

They are deliberately narrower than behavioural testing, resource validation
or general static analysis.

## What the CI Baseline Does Not Guarantee

The current workflow does not run:

- behavioural regression tests;
- interactive shell tests;
- signal-behaviour tests;
- Norminette;
- Valgrind;
- file-descriptor leak checks;
- GCC `-fanalyzer`;
- the Clang Static Analyzer;
- `cppcheck`;
- `clang-tidy`;
- code-coverage reporting.

Those capabilities must not be inferred from a green `CI / build` or
`CI / quality` result.

The maintained compiler-quality and static-analysis evaluation is documented
in:

[`static-analysis.md`](static-analysis.md)

The maintained testing model is documented under:

[`../testing/`](../testing/)

In particular:

- [`../testing/validation-strategy.md`](../testing/validation-strategy.md)
  defines current validation layers and testing gaps;
- [`../testing/manual-validation.md`](../testing/manual-validation.md)
  defines reproducible manual validation procedures.

## Relationship to Quality Automation

The CI baseline now contains two maintained jobs:

    CI / build
    CI / quality

`CI / build` preserves the reference Makefile build through `CC = cc`.

`CI / quality` performs an independent Clang build through a command-line
`CC=clang` override while preserving the same source and Makefile.

The quality job provides compiler diversity. It is not the Clang Static
Analyzer and must not be presented as general static-analysis coverage.

The tooling evaluation and selection rationale are documented in:

[`static-analysis.md`](static-analysis.md)

Separate modernization work can still introduce automated regression testing,
resource-oriented checks or additional analyzer gates where they provide
demonstrated value.

## Relationship to Branch Protection

The maintained repository uses pull requests and protected `main` governance.

The reference build keeps the stable status-check identity:

    CI / build

The maintained compiler-diversity check adds:

    CI / quality

Keeping the workflow name `CI` and the existing `build` job identifier
preserves the reference build identity while allowing quality checks to remain
a distinct concern.

Repository-governance details are documented in:

[`git-workflow.md`](git-workflow.md)

## Local Equivalent

The reference build can be reproduced locally with:

    make fclean
    make
    test -x ./minishell

The compiler-diversity quality build can be reproduced with:

    make fclean
    make CC=clang
    test -x ./minishell

The no-relink property can be checked for the reference build by recording the
executable timestamp, running `make` again and confirming that the timestamp
does not change.

The equivalent quality check uses:

    make CC=clang

for the second build.

Local reproduction validates the build logic.

The actual GitHub Actions runner, workflow permissions, concurrency behaviour
and status checks are validated by the pull-request workflow itself.
