# Static Analysis and Quality Checks

This document describes the maintained compiler-quality baseline, the
static-analysis tools evaluated for Minishell and the reasoning behind the
checks selected for continuous integration.

The current implementation deliberately separates compiler diagnostics from
static analysis.

The authoritative CI workflow is:

[`../../.github/workflows/ci.yml`](../../.github/workflows/ci.yml)

## Purpose

The quality baseline answers a focused question:

> Does the maintained source compile cleanly through an independent C compiler
> while preserving the same build invariants as the reference build?

Different compilers can expose different diagnostics even when they are given
equivalent warning flags.

The current automated quality check does not claim that the program is free of
runtime defects, memory problems, file-descriptor leaks or behavioural bugs.

## Existing Compiler Baseline

The project Makefile preserves the original compiler interface:

    CC = cc
    CFLAGS = -Wall -Wextra -Werror

On the audited Ubuntu 24.04 development environment, `cc` resolves to GCC
13.3.

The maintained reference build therefore remains:

    make

The repository does not change `CC = cc` to GCC or Clang as part of portfolio
modernization.

## Automated Compiler Diversity

The CI workflow contains a separate `quality` job.

Its build is executed with:

    make fclean
    make CC=clang

The command-line `CC=clang` value is a temporary Make variable override.

It does not modify the Makefile and does not change the compiler interface
preserved from the original 42 project.

The quality job validates:

1. a clean Clang build using the existing Makefile;
2. creation of the expected `./minishell` executable;
3. no unnecessary relink when `make CC=clang` is immediately repeated.

The workflow uses the same repository warning flags:

    -Wall -Wextra -Werror

The resulting GitHub status-check identity is:

    CI / quality

## Why Compiler Diversity Was Selected

The audited repository contained approximately:

    69 C translation units
    2 headers
    3650 lines of C source

During the issue #44 discovery audit, all 69 C translation units compiled
successfully with Clang using:

    -Wall -Wextra -Werror

The observed result was:

    69 translation units checked
    0 translation units failed
    0 compiler diagnostics

The complete project also passed:

    make CC=clang
    expected ./minishell artifact
    second make with no relink

This provided useful independent compiler signal while requiring:

- no source changes;
- no Makefile changes;
- no compilation database;
- no analyzer-specific suppression configuration;
- no additional project-level build system.

For the maintained CI baseline, this signal-to-complexity ratio was preferred
over immediately introducing a heavier static-analysis gate.

## Candidate Evaluation

Several quality mechanisms were evaluated before selecting the current
automated check.

### Compiler Warnings

The existing build already uses:

    -Wall -Wextra -Werror

This remains the primary compiler-diagnostic baseline.

Compiler warnings are valuable but do not provide complete static analysis.

### Norminette

Norminette was relevant to the original 42 project and to the issue #44
discovery audit.

That audit was performed before the later source-metadata cleanup removed the
legacy 42 file banners from the maintained C/header tree.

The discovery audit used Norminette 3.3.59 across the then-maintained C source
and headers.

Observed result:

    71 files reported OK
    0 Error! diagnostics
    2 GLOBAL_VAR_DETECTED notices

The notices correspond to the global signal state:

    extern volatile sig_atomic_t g_signal;
    volatile sig_atomic_t g_signal;

This is a deliberate signal-handling design choice.

Despite all files reporting `OK!`, the command returned exit status 1 because
of the notices.

The current maintained tree intentionally no longer contains the mandatory 42
file banners. A repository-wide Norminette run therefore reports header
validation failures and is not a meaningful current acceptance gate.

The recorded issue #44 result remains evidence of the pre-cleanup 42-style
source state. Norminette may still be useful when inspecting the historical
baseline or when explicitly validating 42-specific constraints.

### GCC `-fanalyzer`

GCC's path-sensitive analyzer was evaluated using the existing source and
include structure.

The discovery run checked all 69 C translation units.

Four translation units failed under `-Werror`, producing diagnostics centred
on file-descriptor ownership and process control.

The findings included:

- an apparent double close in the heredoc child/parent path;
- alleged descriptor leaks after `dup2()` in pipeline setup;
- alleged descriptor leaks after `dup2()` during redirection;
- alleged descriptor leaks while restoring standard input and output.

The heredoc diagnostic was investigated against the implementation.

The analyzer modeled a path in which `heredoc_child()` could return to
`exe_heredoc_setup()` and continue into the parent path.

The actual child termination helper ultimately executes:

    exit(status);

Therefore the analyzed child path does not return in runtime execution.

The `dup2()` findings also reflect descriptor ownership that is intentional in
a shell. After duplication to `STDIN_FILENO` or `STDOUT_FILENO`, that standard
descriptor is expected to remain open for subsequent command execution.

Because the current findings require domain-aware interpretation and would make
the workflow fail without appropriate classification or suppression policy,
GCC `-fanalyzer` is retained as an investigative tool rather than a hard CI
gate.

### Clang Static Analyzer

The Clang Static Analyzer was also evaluated across all 69 C translation
units.

Observed result:

    69 translation units checked
    0 command failures
    1 analyzer diagnostic

The diagnostic concerned `libft/ft_memcpy.c` and a possible null destination
dereference.

The maintained call sites examined during the audit provide allocated
destinations before copying, and the normal `memcpy` contract already requires
valid pointer arguments when bytes are copied.

The diagnostic was therefore not demonstrated to represent a current
Minishell runtime defect during this workstream.

The analyzer also introduced additional operational considerations:

- analyzer output files were generated unless output handling was configured;
- warning output did not automatically produce the desired CI failure
  semantics;
- analyzer findings require explicit classification policy.

It is retained as an investigative tool rather than an automated hard gate.

### `cppcheck`

`cppcheck` was considered as an additional independent C static analyzer.

It was not already present in the audited development environment.

The existing compiler and analyzer evaluation produced enough information to
select a useful low-complexity CI check without introducing another dependency.

`cppcheck` is therefore not part of the current baseline.

It can be reconsidered if future source audits demonstrate a concrete need for
additional analyzer coverage.

### `clang-tidy`

`clang-tidy` was also considered.

The repository currently has no:

    compile_commands.json
    .clang-tidy
    CMakeLists.txt

and the audited environment did not contain Bear.

Introducing `clang-tidy` would therefore add tool installation, configuration
and compilation-database considerations before its value for this codebase had
been demonstrated.

It is not part of the current baseline.

## Investigative Tool Reproduction

The investigative commands evaluated during issue #44 are preserved below for
traceability.

Re-running them against the current maintained tree may produce different
results where later repository work intentionally changed the audited
conditions.

### Norminette

During issue #44, the then-maintained C source and headers were checked with:

    norminette include src libft

The recorded result reported all 71 files as `OK!` together with two
`GLOBAL_VAR_DETECTED` notices for the deliberate signal-state global.

Later source-metadata cleanup deliberately removed the legacy 42 file banners.
Running the same command against the current maintained tree therefore produces
header-validation failures and should not be treated as a current repository
acceptance gate.

### GCC `-fanalyzer`

The GCC analyzer discovery checked each C translation unit independently:

    tmp_dir="$(mktemp -d)"

    while IFS= read -r file; do
        gcc -Wall -Wextra -Werror -fanalyzer             -Iinclude -Ilibft             -c "$file"             -o "$tmp_dir/$(basename "$file").o"
    done < <(
        find src libft -type f -name '*.c' -print | sort
    )

    rm -rf "$tmp_dir"

The findings require domain-aware classification and are not currently used as
a hard CI gate.

### Clang Static Analyzer

The Clang Static Analyzer discovery checked each C translation unit
independently:

    while IFS= read -r file; do
        clang --analyze             -Wall -Wextra -Werror             -Iinclude -Ilibft             "$file"
    done < <(
        find src libft -type f -name '*.c' -print | sort
    )

The audited invocation can generate `.plist` analyzer artefacts in the working
directory.

Those generated files are not repository content and can be removed after the
investigation with:

    find . -maxdepth 1 -type f -name '*.plist' -delete

Analyzer output and exit-status semantics must be handled deliberately before
this check could become an automated CI gate.

## Failure Semantics

`CI / quality` fails when:

- required CI dependencies cannot be installed;
- `make fclean` fails;
- the project does not compile successfully with `CC=clang`;
- compiler diagnostics promoted by `-Werror` cause compilation failure;
- the expected `./minishell` executable is not produced;
- an unchanged second `make CC=clang` unexpectedly relinks the executable;
- the job exceeds its configured timeout;
- any workflow step exits unsuccessfully.

A successful `CI / quality` result means that the repository satisfied these
compiler-diversity and build-integration requirements.

It does not mean that separate static analyzers produced no findings.

## Local Reproduction

The automated quality check can be reproduced locally with:

    make fclean
    make CC=clang
    test -x ./minishell

    before="$(stat -c '%y' ./minishell)"
    make CC=clang
    after="$(stat -c '%y' ./minishell)"

    test "$before" = "$after"

The normal reference build remains:

    make fclean
    make

## Guarantees

A successful current CI run provides two complementary build signals:

    CI / build
        reference Makefile build through CC = cc

    CI / quality
        independent Clang build through CC override

Both use the repository's existing warning flags and validate the expected
build artifact and no-relink behaviour.

## What the Quality Baseline Does Not Guarantee

A green `CI / quality` result does not prove:

- behavioural correctness;
- shell grammar correctness;
- memory safety;
- file-descriptor leak freedom;
- signal correctness;
- Norminette compliance;
- absence of GCC `-fanalyzer` findings;
- absence of Clang Static Analyzer findings;
- absence of defects detectable by other static-analysis tools;
- regression coverage;
- code coverage.

These concerns require their own validation mechanisms.

## Relationship to Testing

Compiler diagnostics and static analysis are complementary to testing rather
than replacements for it.

The maintained testing model is documented in:

[`../testing/`](../testing/)

In particular:

- [`../testing/validation-strategy.md`](../testing/validation-strategy.md)
  defines the validation layers and current automation boundaries;
- [`../testing/manual-validation.md`](../testing/manual-validation.md)
  defines reproducible runtime and resource-oriented manual checks.

## Future Evaluation

Static-analysis tools can be reconsidered as the repository evolves.

A future change should only promote an analyzer to a hard CI gate when:

- its findings provide demonstrated value for the maintained codebase;
- known false positives can be handled without hiding important defect classes;
- local and CI execution are reproducible;
- failure semantics are explicit;
- suppressions or exclusions are documented;
- the maintenance cost remains proportional to the benefit.

The later code-quality audit may use the investigated analyzers as diagnostic
tools without automatically making them permanent CI gates.
