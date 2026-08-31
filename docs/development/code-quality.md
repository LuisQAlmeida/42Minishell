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

The initial code-quality audit confirmed that all 71 maintained C/header files
contained the same historical 42 file banner.

Those banners recorded:

    By: jpedro-g <jpedro-g@student.42porto.com>

They were preserved during that audit while the repository's attribution and
historical record were verified.

Git history demonstrates substantial original contribution from both Luís
Quental Almeida and João da Silva, while the README documents the original
two-person development context.

The exact pre-modernization repository state is also preserved by:

    portfolio-baseline-2026-08

After those historical references were confirmed, issue #53 removed the legacy
42 banners uniformly from all 71 maintained C/header files.

The cleanup:

- removed the same 11-line banner and following blank line from every maintained
  C/header file;
- did not selectively alter files according to the contributor named in them;
- did not introduce replacement per-file author or ownership claims;
- did not modify historical commits or the preserved baseline tag;
- did not introduce SPDX identifiers;
- did not change runtime logic or maintained API behaviour.

The current maintained source tree therefore no longer attempts to represent
authorship through decorative per-file metadata.

Authorship and development history remain represented by Git history,
repository documentation and the preserved historical baseline.

Any future SPDX or copyright normalization remains a separate licensing and
attribution decision.

## Technical Comments

The removed historical file banners were distinct from technical
documentation.

Comments that explain non-obvious behaviour, ownership, resource lifecycle,
signals, error handling or interface contracts remain useful engineering
documentation.

The maintained Doxygen comments in `include/minishell.h` and `libft/libft.h`
are not cleanup targets merely because they increase source length.

Comment cleanup should be justified by incorrect, stale, redundant or
misleading content rather than cosmetic preference.

## Resource and Error-Path Findings

A focused review of parent execution, pipelines, heredocs, redirections,
standard-stream backup and process waiting identified the following
failure-path findings.

### `Q48-R1`: pending redirection descriptor after failed input `dup2()`

Classification:

    DEFECT

`exe_redir_apply()` first resolves ownership of the final input and output
redirection descriptors.

During that process, `exe_redir_one()` transfers descriptor ownership away
from the corresponding `t_redir` node by setting:

    r->fd = -1

If both an input and an output descriptor are pending, input is applied first.

If the input `dup2()` fails:

- `exe_redir_dup()` closes the input descriptor;
- `exe_redir_apply()` returns immediately;
- the pending output descriptor remains open;
- its original `t_redir` node no longer owns it.

The pending output descriptor therefore has no remaining cleanup owner.

This is especially relevant in persistent parent-process execution through:

- `blt_execute_parent()`;
- `exe_redir_only()`.

A child-process descriptor leak would disappear when that child exits, while a
descriptor leaked by the shell process can survive subsequent commands.

Resolution:

    #49 Harden parent redirection recovery on dup2 failures

Issue #49 closes the transferred-descriptor ownership gap by explicitly
closing any still-pending output descriptor when input `dup2()` fails.

### `Q48-R2`: `wait()` / `waitpid()` interruption handling

Classification:

    ACCEPTED_TRADEOFF

The execution paths do not explicitly retry `wait()` or `waitpid()` after
`EINTR`.

Before normal child waiting, however, the parent calls `sig_set_waiting()`,
which configures both `SIGINT` and `SIGQUIT` as ignored.

These are the signals explicitly managed by the current shell execution model,
and no maintained application handler was identified that demonstrates a
normal execution path in which these waits are interrupted.

An explicit `EINTR` retry loop could make the implementation more defensive,
but the audit did not demonstrate a current runtime defect that justifies a
behavioural change in this workstream.

### `Q48-R3`: unrecoverable parent standard-stream restoration failure

Classification:

    DEFECT

`exe_stdio_restore()` attempts to restore both saved standard streams through
`dup2()` and then closes both saved descriptors regardless of whether either
restore succeeded.

If a restore operation fails, the function therefore discards the saved
descriptor even though the corresponding original standard stream has not been
successfully restored.

For the persistent shell process, this can leave `STDIN_FILENO` or
`STDOUT_FILENO` in an unexpected state after a failed parent builtin or
redirection-only execution path.

The parent callers also perform restoration after an `exe_redir_apply()`
failure without currently using the restoration return value to override the
existing command status.

Resolution:

    #49 Harden parent redirection recovery on dup2 failures

Issue #49 retries interrupted standard-stream restoration, still attempts both
streams when one restore fails and treats unrecoverable restoration failure as
fatal to the persistent shell session.

This defect shares the same parent-redirection ownership boundary as
`Q48-R1`, so both are resolved through the same focused technical issue.

### `Q48-R4`: Readline operations from the interactive signal handler

Classification:

    PLAUSIBLE_RISK

The interactive `SIGINT` handler records the signal and also calls Readline
functions directly:

    rl_on_new_line()
    rl_replace_line()
    rl_redisplay()

The heredoc handler follows a narrower signal-context pattern based on signal
state, `write()` and descriptor closure.

The audit has not demonstrated a runtime failure caused by the interactive
handler, so this is not classified as a defect.

Changing the signal and Readline integration would also alter interactive
behaviour and requires focused validation beyond this implementation-quality
audit.

The current finding is therefore recorded for future investigation without a
runtime change in #48.

### Resource Ownership Summary

The normal ownership model remains coherent across the audited execution
paths.

Redirection descriptors:

- are initially owned by `t_redir` nodes after preparation;
- transfer ownership when consumed by `exe_redir_one()`;
- use `-1` as the no-owned-descriptor sentinel;
- are closed by `grm_redir_clear()` if ownership remains with a node.

Pipeline descriptors:

- are divided between parent and child after `fork()`;
- have unused ends closed in each process;
- transfer the current read end into `prev_fd` in the parent;
- are closed through `exe_pipe_close()` on pipeline failure.

Heredoc descriptors:

- are created as a pipe before `fork()`;
- have separate inherited copies closed by parent and child;
- transfer the successful read end back to the redirection owner;
- are closed on failed heredoc completion.

Standard-stream backups:

- clean up partial `dup()` success during save;
- retry interrupted restoration attempts;
- attempt restoration of both streams even if one fails;
- consume saved descriptors exactly once;
- terminate the persistent session when restoration can no longer be
  guaranteed.

The audit therefore found no justification for a broad execution refactor.
The demonstrated defects are isolated to parent-process redirection recovery
and are tracked separately.

## Memory Ownership and Allocation Findings

A focused review of token construction, grammar ownership, variable expansion,
environment replacement, persistent shell state and child cleanup found a
generally coherent memory-ownership model.

### `Q48-M1`: literal-dollar allocation failure is not propagated

Classification:

    DEFECT

`expand_dollar_value()` handles a dollar sign that is not followed by a valid
variable-name start by allocating a literal dollar string:

    if (!exp_name_start(str[*i]))
        return (ft_strdup("$"));

Unlike the other allocation paths in the expansion subsystem, failure of this
allocation does not set:

    ERR_MALLOC

If `ft_strdup()` fails, the function therefore returns `NULL` while the error
state remains `ERR_NONE`.

`exp_variables()` can then interpret this as normal expansion progress rather
than as an allocation failure.

For a literal `$`, the allocation failure can therefore be converted into an
empty expansion. For input such as `$-`, the dollar can be silently dropped
while processing continues.

Resolution:

    #50 Propagate allocation failures during literal-dollar expansion

Issue #50 aligns the literal-dollar allocation path with the rest of the
expansion subsystem by setting `ERR_MALLOC` when allocation of the literal
dollar string fails.

### `Q48-M2`: partial `cd` state update after successful `chdir()`

Classification:

    PLAUSIBLE_RISK

`blt_cd()` changes the process working directory before updating the maintained
`OLDPWD` and `PWD` environment entries.

After a successful `chdir()`, later operations can still fail, including:

- `getcwd()`;
- environment-entry allocation;
- `OLDPWD` replacement;
- `PWD` replacement.

A failure can therefore leave the real process working directory changed while
one or both maintained environment entries remain stale or only partially
updated.

The audit did not demonstrate this under normal execution, and making `cd`
fully transactional would require a broader behavioural design decision.

The finding is therefore recorded without a runtime change in #48.

### `Q48-M3`: transient dangling pointers during failure unwind

Classification:

    ACCEPTED_TRADEOFF

Some failure paths free owned memory before the containing structure itself is
abandoned without immediately resetting every pointer to `NULL`.

Examples include scanner token-list cleanup and partially constructed command
argument arrays.

Under the current control flow:

- the owning operation immediately returns after cleanup;
- the containing structure is not reused;
- no second free or subsequent dereference was identified.

Resetting these transient pointers could make future refactoring more
defensive, but no current use-after-free or double-free defect was demonstrated.

### Token Ownership

Token values are transferred into `t_token` nodes when token construction
succeeds.

On failure:

- a newly allocated word is freed if its token node cannot be allocated;
- already-built token lists are released through `scn_token_clear()`;
- token cleanup releases both the owned value and the node.

No token-memory leak or double free was identified in the audited paths.

### Grammar and Command Ownership

Parsed commands own:

- their allocated `argv` array;
- duplicated argument strings;
- redirection nodes;
- duplicated redirection targets.

Partial construction is unwound through the corresponding array and
redirection cleanup functions.

`grm_clear()` remains the final owner cleanup for successfully constructed
command chains.

No unowned command allocation was identified.

### Variable-Expansion Ownership

Expansion builds an owned result buffer from temporary allocated parts.

`sup_join_free_left()` consistently consumes its first argument on both
success and allocation failure.

Temporary expansion parts are freed by the expansion loop after append
attempts.

Apart from `Q48-M1`, allocation failures are propagated through `ERR_MALLOC`
and the accumulated result is released before failure is returned.

### Environment Ownership

The shell owns its mutable `envp` copy for the duration of the session.

Environment extension uses a new pointer array while retaining ownership of
the existing strings.

If allocation of the new entry fails, only the new pointer array is released
and the old environment remains valid.

On successful extension:

- existing string ownership transfers to the new pointer array;
- the old pointer array is freed;
- `shell->envp` is replaced only after successful construction.

Environment removal similarly constructs the replacement pointer array before
freeing the removed entry and old array.

Existing-entry replacement allocates the replacement string before releasing
the previous value.

The audit therefore found the `sta_env_copy()`, `sta_env_set()` and
`sta_env_unset()` ownership transitions coherent under allocation failure.

### Child-Process Cleanup

`exe_child_exit()` releases the child process copies of:

- tokens;
- parsed commands and redirections;
- the shell environment.

Because these allocations exist in the child address space after `fork()`,
their cleanup does not release or invalidate the parent's corresponding
memory.

No cross-process memory-ownership defect was identified.

### Memory Ownership Summary

The audited memory model is generally explicit and stable:

- token nodes own their token values;
- command structures own their duplicated arguments and redirections;
- shell state owns its mutable environment copy;
- expansion temporaries have short, identifiable lifetimes;
- environment replacement preserves the previous valid state until replacement
  allocation succeeds;
- child cleanup operates only on the child's post-`fork()` address space.

The demonstrated allocation-propagation defect is isolated to the
literal-dollar expansion path and is tracked separately in #50.

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

## Implementation Hygiene and Final Validation

The final audit pass reviewed implementation hygiene and reproduced the
maintained quality checks after the resource and memory ownership reviews.

### Source Change Boundary

The code-quality audit introduces no runtime implementation change.

Compared with the `main` baseline, the workstream changes only:

- `docs/development/README.md`;
- `docs/development/code-quality.md`.

No C source, maintained header or Libft implementation file is modified by
this audit.

This is intentional.

The audit identified demonstrated runtime defects, but those defects are
tracked through focused follow-up issues rather than mixed into the audit
branch:

- #49 for parent-process redirection recovery;
- #50 for literal-dollar allocation failure propagation.

### Historical Metadata

The maintained tree contains:

- 69 C source files;
- 2 maintained headers;
- 71 maintained C/header files in total;
- 0 legacy 42 file banners.

Issue #53 removed the previously uniform banners from all 71 maintained files
after historical attribution and preservation were verified.

The transformation was mechanical and repository-wide: each maintained file
lost only its 11-line legacy banner and the following blank line.

No replacement per-file ownership metadata or SPDX identifier was introduced.

### Comment and Maintenance-Marker Hygiene

The 69 maintained C implementation files contain no `//` comment lines and no
block-comment markers.

The maintained C/header tree also contains no:

- `TODO`;
- `FIXME`;
- `XXX`;

maintenance markers.

This does not mean that technical debt is absent. The findings documented in
this audit were established through control-flow, ownership and analyzer review
rather than source annotations.

Maintained API documentation in the headers remains intentionally separate
from this implementation-comment audit.

### Duplicate Helper Review

The only duplicated static helper names found repository-wide remain:

    print_signal_msg
    status_from_wait

Both occur in the standalone-child and pipeline waiting implementations.

Their behavioural difference was reviewed earlier in this audit and remains
classified as:

    ACCEPTED_DUPLICATION

No refactor is justified solely to remove these small local helpers.

### Include Model

The final include inventory remains consistent with the previously documented
historical umbrella-header model.

Most Minishell implementation files include `minishell.h`, while direct
Readline includes are limited to the implementation units that require them.

No broad include redistribution was performed because the audit did not
demonstrate a correctness or maintenance problem sufficient to justify the
repository-wide source churn.

### Reference Build

The final reference build succeeds through the historical Makefile interface:

    make

Result:

- exit status: 0;
- warnings: 0;
- errors: 0;
- Minishell executable generated successfully.

### Compiler-Diversity Build

The final supplemental build also succeeds with:

    make CC=clang

Result:

- exit status: 0;
- warnings: 0;
- errors: 0;
- Minishell executable generated successfully.

### GCC Analyzer Regression

The final GCC 13.3.0 `-fanalyzer` reproduction reports the same six findings
documented by the initial audit:

- two pipeline `dup2()` descriptor warnings;
- one redirection `dup2()` descriptor warning;
- one apparent heredoc double close;
- two standard-stream restoration `dup2()` descriptor warnings.

No new GCC analyzer finding appeared.

The six warnings retain their documented classification as analyzer
limitations and do not justify source changes in this workstream.

### Clang Static Analyzer Regression

The final Clang 18.1.3 analysis covers all 69 C translation units.

Result:

- 69 translation units analyzed;
- 0 non-zero analyzer runs;
- 1 warning.

The remaining warning is the previously documented theoretical
`ft_memcpy()` null-pointer path.

No new Clang Static Analyzer finding appeared.

### Documentation Validation

Doxygen generation succeeds from the maintained repository configuration.

Result:

- Doxygen exit status: 0;
- Doxygen warnings: 0;
- HTML index generated successfully.

Generated documentation remains under the ignored `build/` tree and is not
versioned.

No `.pch`, `.plist` or `.sarif` analyzer artefacts remain in the maintained
source tree.

### Repository Validation

Final repository checks confirm:

- `git diff --check` passes;
- no runtime source file differs from `main`;
- no generated analysis artefact is versioned;
- the worktree is clean after generated build products are ignored;
- static-analysis output is stable against the documented baseline.

### Implementation-Cleanup Decision

No non-functional source cleanup is performed as part of #48.

The audit did not identify an implementation-only change whose demonstrated
maintenance benefit clearly outweighs the churn and regression risk of
modifying otherwise stable source code.

This is a deliberate audit result rather than an incomplete cleanup.

Demonstrated behavioural defects remain isolated in focused follow-up issues,
while accepted risks, trade-offs and analyzer limitations remain documented
for future maintainers.

## Current Baseline

The completed code-quality audit establishes the following baseline:

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
- legacy 42 banners: removed uniformly after historical preservation and attribution were verified;
- broad include redistribution: not justified at this stage;
- generated analyzer artefacts: not versioned;
- parent redirection failure-path defects: resolved by #49;
- `wait()` / `waitpid()` interruption handling: accepted trade-off;
- interactive Readline signal-handler behaviour: plausible risk requiring
  focused future investigation;
- literal-dollar allocation propagation defect: resolved by #50;
- partial `cd` state after post-`chdir()` failure: plausible risk;
- transient dangling pointers during failure unwind: accepted trade-off;
- token, grammar, environment and child-process memory ownership: no
  demonstrated correctness defect;
- runtime implementation changes introduced by #48: none;
- reference and Clang builds: pass with zero warnings;
- GCC and Clang analyzer results: stable against the documented baseline;
- Doxygen generation: pass with zero warnings;
- generated documentation and analyzer artefacts: not versioned;
- implementation-only cleanup: not justified by the audit evidence.

This baseline does not claim that the implementation is defect-free.

It records what was audited, what evidence was obtained and which changes are
or are not currently justified.
