# Engineering Decisions

This directory contains Architecture Decision Records (ADRs) for significant
engineering decisions that shape the maintained Minishell implementation.

The purpose of these records is to preserve architectural rationale separately
from implementation documentation.

For the current as-built system, see
[`../architecture/`](../architecture/).

Historical development material remains under
[`../history/`](../history/).

## What Belongs Here

An ADR should document a decision when:

- it materially affects system architecture or runtime behaviour;
- understanding the rationale helps future maintainers;
- realistic alternatives existed or remain relevant;
- the consequences influence later engineering work.

An ADR should not be created merely because an implementation detail exists.

Routine function structure, naming choices and local refactoring decisions
normally belong in source code, commit history or development documentation
rather than in this directory.

## Source of Truth

ADRs explain why significant engineering choices exist.

They do not override the implementation.

When resolving discrepancies, use the following precedence:

1. current source code for implemented behaviour;
2. maintained architecture documentation for the as-built system model;
3. accepted ADRs for architectural rationale;
4. historical design material for development provenance.

A historical document may explain an earlier intention without describing the
final implementation.

## Retrospective Records

The ADR practice was introduced after the original 42 Minishell implementation
was completed.

The records in this directory may therefore document decisions
retrospectively.

Each ADR distinguishes between:

- implementation facts verified from the final source tree;
- rationale supported by preserved development material;
- rationale reconstructed from the final architecture when no contemporary
  explanation was preserved.

A retrospective ADR must not present reconstructed motivation as a confirmed
historical fact.

The metadata field **Rationale provenance** makes this distinction explicit.

## Naming and Numbering

ADR files use a monotonically increasing four-digit identifier followed by a
short descriptive slug:

```text
ADR-0001-expand-while-scanning.md
ADR-0002-use-linked-command-pipelines.md
```

Numbers are never reused.

If an ADR is superseded, its file remains in place and its status is updated.
A new ADR receives a new number.

The number identifies the record, not the chronological date on which the
original engineering decision was made.

## Status Model

The maintained ADR statuses are:

| Status | Meaning |
| --- | --- |
| `Proposed` | A decision is being considered but has not been adopted. |
| `Accepted` | The decision describes the maintained implementation or an adopted project direction. |
| `Superseded` | A later ADR replaces this decision. |
| `Deprecated` | The decision is retained for provenance but should no longer guide new work. |

The initial Minishell ADR set records decisions already present in the
maintained implementation and will therefore normally use `Accepted`.

## ADR Metadata

Each ADR begins with:

```text
Status:
Recorded:
Decision scope:
Rationale provenance:
```

### `Status`

Uses one value from the ADR status model.

### `Recorded`

The date on which the ADR itself was written.

For retrospective ADRs, this is intentionally not presented as the original
decision date.

### `Decision scope`

Identifies what the record governs.

For the initial ADR set:

```text
Maintained implementation
```

### `Rationale provenance`

Describes how strongly the documented rationale is historically supported.

Two forms are used for the initial retrospective records.

When preserved development material explicitly supports the rationale:

```text
Supported by preserved development material and the final source implementation
```

When the implementation is certain but the original reasoning was not
preserved clearly:

```text
Reconstructed from the final implementation and preserved alternatives
```

The second form must be accompanied by cautious language in the ADR body.

## ADR Structure

Each record should normally follow this structure:

```markdown
# ADR-NNNN: Decision title

- **Status:** Accepted
- **Recorded:** YYYY-MM-DD
- **Decision scope:** Maintained implementation
- **Rationale provenance:** ...

## Context

Describe the architectural problem or constraint.

## Decision

State the decision precisely.

## Rationale

Explain why the decision is appropriate.

Distinguish preserved rationale from retrospective interpretation where
necessary.

## Alternatives Considered

Document alternatives only when they are supported by preserved material or
are necessary to understand the decision.

Do not invent historical debates.

## Consequences

### Positive

Describe benefits created by the decision.

### Trade-offs

Describe limitations, coupling or future costs introduced by the decision.

## Evidence

Identify implementation or preserved documentation that verifies the record.

## References

Link to relevant maintained architecture and historical material.
```

Sections may be adjusted when a decision does not meaningfully require every
heading, but the metadata, context, decision, consequences and evidence should
remain explicit.

## Alternatives and Historical Claims

The phrase **Alternatives Considered** does not imply that every listed
alternative was formally debated during original development.

An ADR must distinguish between:

```text
Preserved project material explicitly considered...
```

and:

```text
The architecture could alternatively have...
```

The second form describes an analytical alternative, not a historical event.

When there is no useful evidence that an alternative mattered, omit it rather
than manufacturing a comparison.

## Decision Index

The following decisions were selected for the initial ADR set after reviewing
the final implementation and preserved design material.

| ADR | Decision | Record state |
| --- | --- | --- |
| [ADR-0001](ADR-0001-expand-while-scanning.md) | Perform variable expansion while quote context is available | Accepted |
| [ADR-0002](ADR-0002-use-linked-command-pipelines.md) | Represent pipelines as linked command chains | Accepted |
| [ADR-0003](ADR-0003-run-standalone-builtins-in-parent.md) | Execute standalone builtins in the parent shell | Accepted |
| [ADR-0004](ADR-0004-use-rolling-pipeline-state.md) | Manage pipelines with rolling descriptor state | Accepted |
| [ADR-0005](ADR-0005-prepare-pipeline-heredocs-before-launch.md) | Prepare pipeline heredocs before launching command children | Accepted |
| ADR-0006 | Maintain an owned mutable environment in shell state | Planned |

`Planned` in this table describes documentation progress, not architectural
status.

Once recorded, each entry will link to its ADR and show its ADR status.

## Initial Evidence Classification

The initial decisions fall into two provenance groups.

### Preserved rationale available

The following decisions have direct support in preserved development material
as well as in the final implementation:

- ADR-0001: expansion while quote context is available;
- ADR-0002: linked command-chain pipeline representation;
- ADR-0003: standalone builtin execution in the parent process.

### Rationale reconstructed cautiously

The following decisions are unambiguous in the final implementation, but their
specific original rationale was not preserved as clearly:

- ADR-0004: rolling pipeline descriptor state;
- ADR-0005: pipeline heredoc preparation before command launch;
- ADR-0006: owned mutable environment state.

Their ADRs must clearly distinguish implementation evidence from reconstructed
rationale.

## Relationship to Architecture Documentation

Architecture documentation answers:

> How does the maintained system work?

ADRs answer:

> Why is a significant architectural choice represented this way?

The two documentation layers should link to each other where useful without
duplicating entire explanations.

Maintained architecture documentation lives under:

[`../architecture/`](../architecture/)

## Relationship to Historical Material

Development-stage design material remains preserved under:

[`../history/design/`](../history/design/)

That material is evidence of project evolution, not automatically the current
source of truth.

Creating an ADR does not rewrite or retroactively correct historical files.

If historical intent differs from the final implementation, the ADR should
identify that distinction explicitly when it is relevant to understanding the
decision.

## Maintenance Rules

When adding or updating ADRs:

- keep one durable decision per record;
- never reuse ADR numbers;
- preserve superseded ADRs;
- distinguish implementation evidence from inferred rationale;
- avoid unsupported claims about original developer intent;
- link to maintained architecture rather than duplicating it wholesale;
- preserve historical documentation unchanged;
- update the decision index when ADR status changes;
- keep ADR changes separate from unrelated source modifications.

A source change that intentionally reverses an accepted architectural decision
should update or supersede the corresponding ADR as part of the same
engineering change whenever practical.
