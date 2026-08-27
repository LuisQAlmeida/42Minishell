# Continuous Integration

This document describes the maintained continuous-integration baseline for the
repository.

The current GitHub Actions workflow provides build integration checks. It is
deliberately narrower than a complete quality gate.

The authoritative workflow configuration is:

[`../../.github/workflows/ci.yml`](../../.github/workflows/ci.yml)

## Purpose

The CI baseline answers a focused question:

> Can the repository be checked out in the maintained CI environment and
> produce the expected Minishell executable through a clean, stable build?

The workflow also checks one important Makefile invariant: rebuilding an
unchanged tree must not relink the final executable.

The workflow does not currently claim behavioural test coverage.

## When CI Runs

The workflow is triggered by:

- pull requests;
- pushes to `main`.

A normal feature-branch push does not run this workflow by itself.

For the maintained contribution lifecycle, this means:

```text
feature branch push
        |
        v
no CI run required yet

pull request opened or updated
        |
        v
CI / build

squash merge to main
        |
        v
CI / build
```

Pull-request runs that are superseded by newer commits are cancelled.

Push runs on `main` are not cancelled by that pull-request policy.

## Execution Environment

The build job currently uses:

```text
ubuntu-24.04
```

The runner version is explicit rather than using `ubuntu-latest`.

This reduces accidental changes to the CI environment when GitHub advances the
meaning of its `latest` runner alias.

The workflow has a job timeout of:

```text
10 minutes
```

A build that exceeds that limit fails rather than occupying a runner
indefinitely.

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

The external system dependency required by the maintained build is:

```text
libreadline-dev
```

The workflow installs it with:

```bash
sudo apt-get update
sudo apt-get install -y --no-install-recommends libreadline-dev
```

GNU Readline is linked by the project Makefile through:

```text
-lreadline
```

The repository's own `libft` is built from the checked-out source tree rather
than installed as a system dependency.

## Build Pipeline

The current `build` job performs the following checks.

### 1. Checkout Repository

The repository is checked out using:

```text
actions/checkout@v4
```

No persisted Git credentials are required after checkout.

### 2. Install Build Dependencies

The workflow installs the GNU Readline development package required to compile
and link Minishell.

### 3. Clean Build

The workflow executes:

```bash
make fclean
make
```

Starting from `make fclean` makes the build check explicit even though GitHub
Actions already provides a fresh checkout for each job.

The build therefore validates that the project can be rebuilt from its own
clean state rather than succeeding only because generated objects already
exist.

### 4. Verify the Build Artifact

After compilation, the workflow checks:

```bash
test -x ./minishell
```

A successful compiler exit alone is not treated as sufficient if the expected
executable is missing or is not executable.

### 5. Verify No Relink

The workflow records the modification timestamp of `./minishell`, executes:

```bash
make
```

again and compares the timestamp afterward.

If the timestamp changes, CI fails.

This checks that an unchanged repository does not unnecessarily relink the
final executable.

The local discovery audit for the CI workstream confirmed that the current
Makefile already satisfies this property:

```text
make: Nothing to be done for 'all'.
```

The workflow therefore validates an existing Makefile invariant. It does not
modify the Makefile to create one.

## Failure Semantics

A failed `CI / build` check means that at least one maintained build-integration
requirement was not satisfied.

Examples include:

- the dependency installation failed;
- `make fclean` failed;
- the clean build failed;
- the expected `minishell` executable was not produced;
- the executable was not executable;
- an unchanged second `make` relinked the executable;
- the build exceeded the configured timeout;
- another workflow step terminated unsuccessfully.

A failed build check does not identify every possible runtime defect.

Conversely, a successful build check does not prove complete Minishell
correctness.

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

A successful current CI run provides evidence that:

- the repository can be checked out by GitHub Actions;
- the declared build dependency can be installed;
- the project can complete a clean build on Ubuntu 24.04;
- the expected `minishell` executable is produced;
- the executable is marked executable;
- an unchanged second `make` does not relink the executable.

These are build-integration guarantees only.

## What CI Does Not Currently Guarantee

The current workflow does not run:

- behavioural regression tests;
- interactive shell tests;
- signal-behaviour tests;
- Norminette;
- Valgrind;
- file-descriptor leak checks;
- static analysis;
- code-coverage reporting.

Those capabilities must not be inferred from a green `CI / build` result.

The maintained testing model is documented under:

[`../testing/`](../testing/)

In particular:

- [`../testing/validation-strategy.md`](../testing/validation-strategy.md)
  defines current validation layers and testing gaps;
- [`../testing/manual-validation.md`](../testing/manual-validation.md)
  defines reproducible manual validation procedures.

## Relationship to Future Quality Automation

This CI baseline intentionally provides a stable foundation rather than
collecting every future quality check into one workstream.

Separate modernization work can later introduce:

- automated regression testing;
- static analysis and other quality checks;
- additional CI jobs where they provide demonstrated value.

When those capabilities are actually implemented, this document and the
testing documentation should be updated to describe the expanded CI contract.

## Relationship to Branch Protection

The maintained repository uses pull requests and protected `main` governance.

The workflow's stable identity remains:

```text
CI / build
```

Keeping the workflow name `CI` and job identifier `build` avoids unnecessarily
changing the status-check identity while the baseline itself is improved.

Repository-governance details are documented in:

[`git-workflow.md`](git-workflow.md)

## Local Equivalent

The central build behaviour can be reproduced locally with:

```bash
make fclean
make

test -x ./minishell

before="$(stat -c '%y' ./minishell)"
make
after="$(stat -c '%y' ./minishell)"

test "$before" = "$after"
```

The GitHub-specific parts, including runner selection, permissions,
pull-request concurrency and status-check behaviour, require an actual GitHub
Actions run for final validation.
