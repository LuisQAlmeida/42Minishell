# Architecture

This directory contains the maintained architecture documentation for
Minishell.

The documents describe the current implementation as verified against the
source tree. They are intended to help a developer understand the system at
different levels without requiring a line-by-line reading of the code.

## Documentation Map

| Document | Question it answers |
| --- | --- |
| [`system-overview.md`](system-overview.md) | What are the main subsystems, data structures and architectural boundaries? |
| [`runtime-flow.md`](runtime-flow.md) | What happens to one input line from prompt to final exit status? |
| [`process-and-signals.md`](process-and-signals.md) | Which process executes each kind of work, and which signal policy applies? |
| [`resource-ownership.md`](resource-ownership.md) | Who owns memory and file descriptors, when does ownership move, and where are resources released? |

## Recommended Reading Order

For a first pass through the implementation:

1. [`system-overview.md`](system-overview.md)
2. [`runtime-flow.md`](runtime-flow.md)
3. [`process-and-signals.md`](process-and-signals.md)
4. [`resource-ownership.md`](resource-ownership.md)

The documents are intentionally independent enough to be consulted
individually.

For example:

- use the system overview when navigating the source tree;
- use the runtime flow when tracing a command;
- use the process and signal model when debugging process behaviour;
- use the ownership model when reasoning about memory or file-descriptor
  lifetime.

## Source of Truth

Architecture documentation in this directory describes the maintained
implementation.

When resolving discrepancies, use the following precedence:

1. current source code;
2. maintained architecture documentation in this directory;
3. the repository-level README for public overview;
4. historical development material.

The source code remains authoritative for implemented behaviour.

Architecture documentation explains that behaviour at subsystem and runtime
level.

## Current Architecture

The implemented command lifecycle is:

```text
interactive input
        |
        v
session
        |
        v
scan + quote-aware expansion
        |
        v
token list
        |
        v
grammar
        |
        v
linked command chain
        |
        v
simple or pipeline execution
        |
        v
exit status
        |
        v
persistent shell state
```

The major maintained source domains are:

```text
src/
├── session/
├── scan/
├── expand/
├── grammar/
├── exec/
├── builtins/
├── state/
├── signals/
└── support/
```

The architecture deliberately follows the final implementation rather than
earlier design intentions.

## Documentation Boundaries

The documents in this directory cover:

- subsystem responsibilities;
- major runtime flows;
- core data structures;
- process boundaries;
- signal contexts;
- persistent and transient state;
- memory ownership;
- file-descriptor ownership;
- architectural constraints.

They do not attempt to:

- document every function;
- duplicate implementation details already clear from source;
- serve as API reference documentation;
- record the rationale for every engineering decision;
- rewrite historical project material.

Engineering rationale belongs in
[`../decisions/`](../decisions/).

Detailed verification belongs in
[`../testing/`](../testing/).

## Historical Architecture

Earlier design and development-stage architecture documents are preserved under:

[`../history/design/`](../history/design/)

They provide useful provenance but are not automatically authoritative for the
final implementation.

Notable differences between historical design material and the maintained
implementation include:

- variable expansion occurs during scanning while quote context is available;
- pipelines are represented as linked `t_cmd` nodes rather than a separate
  pipeline AST;
- mutable environment state is stored directly as an owned `char **` in
  `t_shell`;
- pipeline construction uses rolling descriptor state;
- source-module boundaries evolved during implementation.

Historical documents should remain unchanged unless a separate historical
correction is explicitly required.

## Maintenance Rules

Architecture documentation should remain:

- aligned with implemented behaviour;
- explicit about current limitations;
- organized by architectural concern rather than source-file chronology;
- linked to related maintained documentation;
- independent from historical design material;
- concise enough that each document has a clear purpose.

When implementation changes invalidate an architectural claim, update the
relevant maintained document in the same change whenever practical.

New architecture documents should be added only when they introduce a distinct
and durable concern that cannot be expressed clearly in the existing set.
