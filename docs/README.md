# Documentation

This directory is the documentation entry point for the Minishell repository.

The documentation is organized by purpose and lifecycle so that current
engineering information remains clearly separated from historical project
artifacts.

## Start Here

| Area | Purpose | Status |
| --- | --- | --- |
| [Architecture](architecture/) | Current system structure, runtime flows and subsystem relationships | Active documentation domain |
| [Engineering Decisions](decisions/) | Significant technical decisions and Architecture Decision Records | Active documentation domain |
| [Development](development/) | Current contribution, maintenance and repository workflow | Active documentation domain |
| [Testing and Validation](testing/) | Current verification strategy and maintained test documentation | Active documentation domain |
| [Project History](history/) | Original requirements, design history, planning, validation evidence and previous team workflow | Historical |

The active documentation domains currently establish the repository
documentation contract. Detailed documents will be added or promoted only
after they are reviewed against the maintained implementation.

## Documentation Model

The repository separates maintained documentation from historical material:

```text
docs/
├── architecture/   current system design
├── decisions/      accepted engineering decisions
├── development/    current development workflow
├── testing/        current verification strategy
└── history/        original and superseded project artifacts
```

This separation prevents development-era plans or earlier design assumptions
from being mistaken for descriptions of the current codebase.

## Source of Truth

Different information has different authoritative sources:

- the source code is authoritative for implemented behaviour;
- current documentation explains and contextualizes the maintained
  implementation;
- the root [`README.md`](../README.md) provides the public project overview;
- accepted ADRs record significant engineering decisions and their rationale;
- historical documentation preserves provenance but is not automatically
  authoritative for the maintained implementation;
- the original 42 subject records the requirements of the original assignment,
  not the complete lifecycle of the repository after that project ended.

When current documentation and the implementation disagree, the discrepancy
should be investigated and the documentation corrected or explicitly marked as
historical.

## Documentation Lifecycle

### Current

Documentation under `architecture/`, `decisions/`, `development/` and
`testing/` is intended to describe the maintained repository.

Content should only be promoted into these areas after its accuracy has been
reviewed.

### Historical

Documentation under [`history/`](history/) records earlier stages of the
project.

Historical material may include:

- original requirements;
- proposed or intermediate designs;
- implementation plans;
- evaluation preparation;
- validation evidence;
- previous collaboration and project-management practices.

Historical documents are preserved for traceability rather than silently
rewritten to look current.

### Superseded or unused material

Documentation that no longer represents the repository should either:

1. be preserved under `history/` when it contains useful project knowledge; or
2. be removed when it is only unused scaffolding or duplication.

Git history remains the final record of removed documentation.

## Documentation Principles

Documentation in this repository should be:

- accurate before comprehensive;
- close to the implementation it describes;
- explicit about whether it is current or historical;
- useful to a developer unfamiliar with the 42 curriculum;
- concise enough to remain maintainable;
- linked rather than unnecessarily duplicated;
- versioned alongside the code.

Documentation structure should grow when information needs a durable home, not
to imitate organizational complexity that the project does not require.

## Planned Documentation Work

The repository modernization initiative will develop the active domains
incrementally, including:

- current architecture and runtime-flow documentation;
- engineering decision records;
- contribution and Git workflow documentation;
- maintained testing and validation strategy;
- automated quality and CI documentation;
- API documentation where it adds value.

Until those workstreams are completed, historical material remains available
under [`history/`](history/) as evidence and context.
