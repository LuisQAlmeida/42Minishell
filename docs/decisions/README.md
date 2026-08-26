# Engineering Decisions

This directory records significant engineering decisions that affect the
architecture, behaviour, maintainability or development of the project.

Decisions are documented as Architecture Decision Records (ADRs) when the
reasoning behind them is valuable to future contributors.

## ADR Format

ADR files follow the naming convention:

```text
ADR-0001-short-title.md
ADR-0002-short-title.md
```

Each ADR should normally contain:

- status;
- date;
- context;
- decision;
- alternatives considered;
- consequences;
- relevant references.

## Lifecycle

Accepted ADRs describe decisions that apply to the maintained implementation.

Historical design notes and decisions that have not yet been verified against
the final implementation remain under [`../history/`](../history/) until they
are reviewed.

Detailed ADRs will be introduced through the dedicated engineering-decisions
workstream.
