# API Documentation Strategy

This document defines the maintained API-documentation boundary for Minishell
and the role that Doxygen plays within the repository.

## Purpose

The goal of API documentation is to make meaningful contracts, shared data
structures, ownership rules and subsystem boundaries easier to understand.

It is not intended to mechanically document every function or implementation
detail.

The hand-written architecture documentation remains authoritative for
system-level design.

Doxygen provides a navigable reference for selected interfaces.

## Documentation Boundary

The maintained Doxygen boundary is primarily defined by:

    include/minishell.h
    external/libft/libft/libft.h

The central Minishell header contains interfaces shared between the program's
session, scanner, grammar, expansion, execution, builtin, state, signal and
support subsystems.

The canonical Libft header contains Doxygen-style documentation for its
utility interface.

Implementation files under `src/` remain the Minishell-owned source of
function definitions. Libft implementation files are maintained separately in
the pinned dependency under `external/libft/`; implementation-local helpers
from either codebase are not part of the documented API by default.

## Core Data Model

The following types form the shared data model and should be documented:

- `t_toktype`
- `t_err`
- `t_token`
- `t_redir`
- `t_cmd`
- `t_shell`

These types cross subsystem boundaries and represent the principal vocabulary
used to move shell state, tokens, commands, redirections and errors through
the runtime pipeline.

The following structures are subsystem-local implementation details and should
not normally be exposed as part of the maintained API reference:

- `t_tokctx`
- `t_child_ctx`
- `t_pipe_state`

They may still receive targeted comments where a non-obvious invariant or
ownership rule needs explanation.

## Global Signal State

The declaration:

    extern volatile sig_atomic_t g_signal;

is part of the documented cross-module contract.

The documentation should explain its role as signal state shared between the
signal handlers and the session or execution logic.

Documentation must not imply that the variable is a general-purpose global
state mechanism.

## Cross-Module Interfaces

Functions deserve API documentation when their contract is relevant outside
the subsystem that defines them.

Current cross-module candidates include the following categories.

### Runtime pipeline

- `ses_loop`
- `scn_line`
- `grm_pipeline`
- `exe_simple`
- `exe_pipeline`

These functions connect major runtime phases and should describe their inputs,
outputs, error behaviour and ownership expectations.

### Expansion

- `exp_variables`

The documentation should describe allocation ownership, shell-state
dependency and error reporting.

### Builtin integration

- `blt_is_builtin`
- `blt_execute`
- `blt_execute_parent`

Documentation should explain dispatch semantics and, where relevant, why some
builtins execute in the parent shell process.

### Environment state

- `sta_env_copy`
- `sta_env_entry`
- `sta_env_set`
- `sta_env_unset`
- `sta_env_value`

Documentation should make allocation and ownership rules explicit where they
are not obvious from the signature.

### Execution and descriptor state

- `exe_redir_apply`
- `exe_stdio_save`
- `exe_stdio_restore`

Documentation should focus on descriptor ownership, side effects and restore
expectations rather than restating implementation steps.

### Signals

- `sig_set_interactive`
- `sig_set_heredoc`
- `sig_set_waiting`
- `sig_set_child`

Documentation should explain which signal mode each function establishes and
the runtime context in which that mode is expected.

### Shared cleanup and support

- `scn_token_clear`
- `grm_clear`
- `sup_free_array`
- `sup_join_free_left`

Documentation should emphasize ownership transfer or destruction semantics.

Simple helpers such as `sup_is_space` or counting utilities do not require
heavy API documentation merely because they happen to be used from multiple
subsystems.

## Internal Interfaces

A function is not automatically part of the documented API merely because it
is declared in `minishell.h`.

Many declarations exist there because the original project uses a central
header for both subsystem interfaces and implementation plumbing.

Single-subsystem functions and file-local static helpers should remain
undocumented unless a non-obvious contract provides genuine maintenance value.

The repository currently contains 79 static source helpers. They are explicitly
not a target for mechanical Doxygen coverage.

## Documentation Conventions

Documentation should explain information that cannot be recovered reliably
from the function or type name alone.

Useful subjects include:

- ownership transfer;
- allocation responsibility;
- destruction responsibility;
- lifecycle expectations;
- valid state transitions;
- process ownership;
- file-descriptor ownership;
- signal-mode expectations;
- error reporting;
- side effects;
- assumptions that callers must satisfy.

Documentation should not merely restate obvious code.

For example, a comment saying that a function named `sta_env_copy` copies an
environment is low-value.

A useful comment explains who owns the returned environment, how allocation
failure is reported and who is responsible for releasing it.

## Doxygen Policy

The intended generated reference should favor documented interfaces over
exhaustive extraction.

The configuration should therefore:

- avoid treating every declaration as documented automatically;
- avoid extracting static implementation helpers by default;
- avoid committing generated HTML output;
- keep configuration in the repository;
- allow generation with a single documented local command;
- treat warnings as information to review deliberately rather than silently
  hiding documentation problems;
- remain lightweight enough for contributors to understand without a separate
  documentation build system.

The maintained documentation source boundary focuses on the repository's
interface headers rather than presenting every implementation file as public
API.

The generated API currently publishes:

- `include/minishell.h`, containing the curated Minishell data model and
  selected cross-module contracts;
- `external/libft/libft/libft.h`, containing the documented utility API used
  by Minishell;
- this document, used as the generated documentation main page.

Both maintained headers include Doxygen `@file` documentation.

With `EXTRACT_ALL = NO`, the audited Doxygen 1.9.8 configuration did not
publish documented global functions, variables, enums and typedefs as file or
global API pages until their containing headers were documented with `@file`.

The `@file` declarations are therefore part of the maintained publication
boundary rather than decorative file comments.

## Libft Utility API

`external/libft/libft/libft.h` contains the Doxygen-style contracts provided
by the canonical Libft dependency.

Before the post-`v1.0.0` dependency externalization, the maintained Minishell
tree carried an equivalent bundled Libft subset under `libft/`. That duplicate
implementation was removed after direct source comparison and external-build
validation demonstrated that Minishell could consume the canonical dependency
without changing its intended runtime behaviour.

The canonical header remains an explicit Doxygen input and its documented
functions are published alongside the Minishell API.

This keeps the Libft contracts discoverable while maintaining a clear ownership
boundary between the Minishell superproject and its external dependency.

## Historical 42 Headers

The maintained C/header tree originally contained the legacy 42 file banner in
all 71 maintained source and header files.

Those banners recorded `jpedro-g` in their per-file author, creation and update
metadata. They were preserved during the initial modernization and API
documentation audits while repository attribution and historical preservation
were reviewed.

The repository now preserves the original development context through:

- Git history;
- collaborative attribution in repository documentation;
- historical project documentation;
- the immutable pre-modernization tag `portfolio-baseline-2026-08`.

After that historical record was verified, the legacy banners were removed
uniformly from the current maintained tree.

The removal does not rewrite historical commits or the preserved baseline and
does not replace the banners with new per-file ownership claims.

No SPDX identifier was introduced as part of the banner cleanup. Any future
SPDX or copyright normalization remains a separate licensing and attribution
decision.

Historical source metadata is therefore not part of the maintained API
documentation boundary.

## Generated Documentation

Generated Doxygen artefacts are build products.

They should not be committed to the repository.

The repository should contain:

- the Doxygen configuration;
- source-level API comments;
- documentation-generation instructions.

The generated HTML or other output should remain local unless a later
workstream deliberately introduces hosted documentation.

## Local Generation

The maintained API reference requires Doxygen.

On Ubuntu, the required package can be installed with:

    sudo apt install doxygen

The configuration is stored in the repository root as:

    Doxyfile

Generate the documentation from the repository root with:

    mkdir -p build/docs/doxygen
    doxygen Doxyfile

Doxygen writes the HTML reference to:

    build/docs/doxygen/html/

The entry point is:

    build/docs/doxygen/html/index.html

The repository already ignores `build/`, so generated documentation remains a
local build artefact and must not be committed.

The output directory is created explicitly because the audited Doxygen 1.9.8
installation does not create the complete nested `build/docs/doxygen`
directory hierarchy when its parent directories do not yet exist.

The maintained configuration deliberately uses:

    EXTRACT_ALL = NO
    EXTRACT_STATIC = NO

This keeps generated documentation aligned with the selective API boundary
rather than automatically treating implementation details as public
interfaces.

Documentation generation is independent from the historical project Makefile.
No `make docs` target is required by the current baseline.

## Relationship to Architecture Documentation

Generated API documentation answers questions such as:

- what data does this type contain?
- what does this interface require?
- who owns the returned resource?
- what side effects occur?
- what error state can be produced?

The hand-written architecture documentation answers broader questions such as:

- how does a command move through the shell?
- why are modules separated this way?
- how do parsing, execution and state interact?
- what design decisions shaped the implementation?

Doxygen supplements that documentation.

It does not replace it.
