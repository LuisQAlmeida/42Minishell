# Code Quality

This document records the maintained code-quality baseline for Minishell and
the findings of the repository-wide implementation audit introduced during the
portfolio modernization.

The purpose of this audit is not to refactor working code for stylistic
preference. It distinguishes demonstrated defects from analyzer limitations,
accepted implementation trade-offs, historical metadata and cleanup that
materially improves maintainability.

## Audit Scope

The maintained implementation currently contains:

- 69 C source files;
- 2 maintained headers;
- 71 maintained C/header files in total;
- 83 static helper functions across `src/` and `libft/`;
- 93 functions declared through the maintained headers.

The audit covers:

- compiler diagnostics;
- static-analysis findings;
- resource and descriptor ownership;
- process and signal handling;
- error paths;
- declarations and apparent dead code;
- implementation duplication;
- include/dependency hygiene;
- technical comments;
- historical 42 source metadata.

Runtime behaviour remains authoritative in the source code.

## Compiler Baseline

The maintained implementation builds successfully with the historical compiler
interface:

    make

The Makefile continues to use:

    CC = cc

The same implementation also builds successfully through the supplemental
compiler-diversity check:

    make CC=clang

At the audited baseline:

- the reference `cc` build succeeds;
- the Clang build succeeds;
- both builds use `-Wall -Wextra -Werror`;
- neither build reports compiler warnings.

Changing the historical Makefile compiler interface is not required by this
audit.

## Declaration and Dead-Code Audit

The maintained headers declare 93 functions.

A repository-wide occurrence audit found:

- no declared function without an implementation;
- no declared function with zero apparent callers;
- no unresolved maintained declarations.

A number of functions have a single caller. These are predominantly legitimate
cross-module boundaries or small subsystem interfaces and are not evidence of
dead code.

The audit therefore found no justified function or prototype removal at this
stage.

## Static Helper Inventory

The repository contains 83 static functions:

- 79 under the Minishell implementation in `src/`;
- 4 inside the bundled Libft implementation.

The Libft static helpers account for the difference between this
repository-wide count and the earlier Minishell-only API audit.

No discrepancy in implementation history is implied by the two counts.

## Explicit Maintenance Markers

The maintained C/header tree contains:

- 0 `TODO` markers;
- 0 `FIXME` markers.

The absence of these markers does not imply that the implementation is free of
technical debt. It means that quality findings must be established through
code review, testing and analysis rather than collected from explicit source
annotations.

## Resource Hotspots

The implementation currently contains:

- 3 `fork()` call sites;
- 9 `dup()` / `dup2()` call sites;
- 23 `close()` call sites;
- 1 direct `exit()` call site.

Resource-sensitive implementation is concentrated in:

- child execution;
- pipelines;
- heredocs;
- redirection handling;
- standard-stream backup and restoration.

These areas receive explicit review because descriptor ownership crosses
process and execution boundaries.

## GCC Analyzer Findings

The audit was reproduced with GCC 13.3.0 using:

    -Wall -Wextra -fanalyzer

The analyzer build succeeds and reports six descriptor-related warnings.

### `pipeline_child.c`: duplicated standard input

GCC reports a possible descriptor leak for the `dup2()` operation that installs
the previous pipeline descriptor as `STDIN_FILENO`.

Classification:

    ANALYZER_LIMITATION

The duplicated standard descriptor is intentionally retained as the process
standard input. It is not a temporary descriptor that should be closed
immediately after successful `dup2()`.

### `pipeline_child.c`: duplicated standard output

GCC reports the equivalent warning when the current pipeline write descriptor
is installed as `STDOUT_FILENO`.

Classification:

    ANALYZER_LIMITATION

The resulting standard output descriptor must remain available to the child
process and subsequent `exec` path.

### `redir_apply.c`: redirection `dup2()`

GCC reports a descriptor leak on the standard descriptor created by
`exe_redir_dup()`.

Classification:

    ANALYZER_LIMITATION

The original redirection descriptor is explicitly closed. The duplicated
standard descriptor is intentionally retained as the effective process stream.

### `stdio_backup.c`: restored standard input and output

GCC reports two possible descriptor leaks when saved standard descriptors are
duplicated back onto `STDIN_FILENO` and `STDOUT_FILENO`.

Classification:

    ANALYZER_LIMITATION

The saved descriptors are closed by `exe_stdio_restore()`. The restored
standard descriptors are intentionally left open because they remain the
process standard streams.

### `heredoc.c`: apparent double close

GCC reports a possible double close of the heredoc pipe write descriptor.

Classification:

    ANALYZER_LIMITATION

The close operations occur in different processes after `fork()`. Parent and
child each own a copy of the inherited file-descriptor table and must close
their respective copy when it is no longer required.

The analyzer models this path conservatively and does not represent this
process separation as two independent descriptor tables.

No runtime change is justified by the current GCC analyzer findings.

## Clang Static Analyzer Finding

The audit was reproduced with Clang 18.1.3 over the 69 C translation units.

The analyzer reports one code finding:

    libft/ft_memcpy.c
    possible null-pointer dereference

The warning follows the abstract input state:

    dest == NULL
    src != NULL
    n > 0

Under those inputs, dereferencing `dest` would indeed be invalid.

Classification:

    ACCEPTED_TRADEOFF

The current Minishell call sites satisfy the valid `ft_memcpy()` preconditions.

The audited callers are:

- `sup_join_free_left()`, where the destination allocation is checked before
  copying and each source is copied only when its measured length is non-zero;
- `join_path_cmd()`, where the destination allocation is checked before
  copying the supplied path component.

No current application path was identified that supplies a null source or
destination with a positive copy length.

Changing `ft_memcpy()` to define additional behaviour for invalid pointer
inputs solely to silence the analyzer would modify its contract without fixing
a demonstrated Minishell defect.

## Wait-Status Helper Duplication

`src/exec/wait_child.c` and `src/exec/pipeline_wait.c` each contain static
helpers named:

    print_signal_msg()
    status_from_wait()

The implementations are similar but intentionally differ in signal-reporting
behaviour.

A standalone child converts its wait status and reports a terminating signal
during that conversion.

Pipeline waiting separates status conversion from signal reporting so that a
signal message is emitted once for the pipeline rather than once for every
signaled process.

Classification:

    ACCEPTED_DUPLICATION

Introducing shared abstraction solely to eliminate these small helpers would
add coupling without a demonstrated maintainability or correctness benefit.

## Include and Dependency Model

Most Minishell source files include the central `minishell.h` header.

The header therefore acts partly as a historical umbrella header in addition
to containing maintained cross-module declarations.

Some dependencies are directly required by public declarations, including
types associated with:

- `pid_t`;
- `sig_atomic_t`;
- `struct termios`.

Other system headers primarily support implementation details in individual C
files.

A broad include redistribution is not currently justified because it would
touch many otherwise stable source files without fixing a demonstrated
correctness or maintenance problem.

Dependency hygiene may be revisited if later changes create a concrete reason
to narrow header exposure.

## Historical 42 File Banners

All 71 maintained C/header files contain the historical 42 banner.

All currently record:

    By: jpedro-g <jpedro-g@student.42porto.com>

The banners are uniform historical metadata rather than maintained ownership
or API documentation.

Git history, the historical baseline tag and repository documentation preserve
the collaborative origin of the project and show contributions from both
original developers.

The repository license currently contains a copyright notice for Luís Quental
Almeida, while Git history also demonstrates substantial original contribution
from João da Silva.

Because SPDX license identifiers do not replace attribution or copyright
metadata, this workstream does not mechanically replace the historical banners
with SPDX comments.

Maintained policy:

- preserve the 71 historical banners unchanged in the current modernization
  baseline;
- do not selectively rewrite banners according to the contributor named in
  them;
- do not replace them with new per-file ownership claims;
- preserve actual authorship through Git history and repository history
  documentation;
- treat any future SPDX/copyright migration as a separate licensing and
  attribution decision.

## Technical Comments

Historical file banners are distinct from technical documentation.

Comments that explain non-obvious behaviour, ownership, resource lifecycle,
signals, error handling or interface contracts remain useful engineering
documentation.

The maintained Doxygen comments in `include/minishell.h` and `libft/libft.h`
are not cleanup targets merely because they increase source length.

Comment cleanup should be justified by incorrect, stale, redundant or
misleading content rather than cosmetic preference.

## Finding Classification

Quality findings use the following classifications:

### `DEFECT`

A demonstrated correctness problem requiring a runtime change.

Behaviour-changing defects should normally be handled through a separate,
focused technical issue.

### `PLAUSIBLE_RISK`

A credible failure mode that has not yet been demonstrated and requires
additional investigation.

### `ANALYZER_LIMITATION`

A warning caused by limitations or conservative assumptions in static-analysis
modelling where the audited runtime contract remains valid.

### `ACCEPTED_TRADEOFF`

A real implementation property or limitation that is understood and currently
accepted because changing it is not justified by demonstrated risk or
maintenance benefit.

### `ACCEPTED_DUPLICATION`

Deliberate or low-risk duplication whose removal would introduce more
complexity or coupling than the duplication itself.

## Current Baseline

The initial code-quality audit establishes the following baseline:

- reference `cc` build: pass;
- Clang compiler-diversity build: pass;
- compiler warnings: none;
- declared functions without apparent callers: none;
- unresolved maintained declarations: none;
- explicit `TODO` / `FIXME` markers: none;
- GCC analyzer descriptor findings: classified as analyzer limitations;
- Clang `ft_memcpy()` finding: accepted trade-off under the maintained call
  contract;
- duplicated wait helpers: accepted duplication;
- historical 42 banners: preserved uniformly;
- broad include redistribution: not justified at this stage;
- generated analyzer artefacts: not versioned.

This baseline does not claim that the implementation is defect-free.

It records what was audited, what evidence was obtained and which changes are
or are not currently justified.
