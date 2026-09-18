# Project History

This directory preserves documentation from the original collaborative
Minishell project.

Most artifacts in this tree were created during the academic development phase
and were later moved here during portfolio modernization without rewriting
their historical content.

They are retained as evidence of how the project was designed, planned,
validated and managed at the time.

Historical documentation should not automatically be interpreted as a
description of the current implementation or repository workflow.

## Contents

### `original-requirements/`

Subject provenance for the original 42 Porto Minishell project.

The original project used **Minishell subject version 10.0**. The PDF that was
tracked during the academic project is no longer redistributed from the
maintained public tree, but remains recoverable through Git history and the
immutable `portfolio-baseline-2026-08` tag.

Maintained evaluation and subject metadata live under
[`../academic/`](../academic/).

### `design/`

Project-era architecture and implementation-flow documentation.

These files preserve intermediate and final design thinking from the original
development phase. Some details differ from the maintained implementation and
are intentionally not rewritten to match later portfolio work.

The maintained ADRs under [`../decisions/`](../decisions/) reference selected
historical design documents as evidence of earlier engineering decisions.

### `planning/`

Project-specific implementation planning artifacts.

The preserved MSH-12 expansion plan records scope, dependencies, design
questions, acceptance criteria, planned commits and validation expectations
used during development.

### `validation/`

Project-era manual test matrices and validation evidence.

These documents cover broad mandatory behaviour as well as focused expansion
and builtin work. They remain historical evidence rather than claims that the
current maintained tree has automatically rerun every recorded check.

Current validation guidance lives under [`../testing/`](../testing/).

### `team-workflow/`

Evidence of the original Luís/João collaboration model.

The retained material records:

- working agreements;
- Definition of Ready;
- Definition of Done;
- Jira key and traceability policy;
- GitHub collaboration and branch-protection setup.

These files document the original project workflow and are not current
repository-maintenance policy.

Current contribution guidance lives in
[`../../CONTRIBUTING.md`](../../CONTRIBUTING.md).

## Curation Principle

Historical value comes from evidence of how the project was actually developed,
not from preserving every generic tutorial or operational reference that once
lived in the repository.

Project-specific engineering, planning, validation and collaboration evidence
is retained.

Generic helper material may be removed from the maintained tree when stronger
project-specific evidence already preserves the relevant history.

Removed historical files remain recoverable through Git history and the
immutable pre-modernization baseline:

```text
portfolio-baseline-2026-08
```

## Documentation Lifecycle

The repository keeps current and historical documentation separate:

```text
current documentation
        |
        |-- architecture/
        |-- decisions/
        |-- development/
        `-- testing/

historical documentation
        |
        `-- history/
```

Current documentation may use historical material as engineering evidence
without rewriting that material to match later implementation or maintenance
decisions.
