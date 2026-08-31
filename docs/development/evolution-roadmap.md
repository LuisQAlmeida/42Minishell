# Post-Baseline Evolution Roadmap

This document defines how the maintained Minishell repository may evolve after
the preserved historical 42 project baseline.

It is a decision framework rather than a commitment to implement every
possible improvement.

Its purpose is to keep later maintenance, correctness fixes and portfolio work
clearly distinguishable from the original 42 project.

## Baseline Model

The repository has two intentionally different baseline concepts.

### Historical 42 Baseline

The exact repository state immediately before professional portfolio
modernization is preserved by the annotated tag:

    portfolio-baseline-2026-08

That tag resolves to the original project-era state and is treated as an
immutable historical reference.

It preserves:

- the original implementation;
- project-era source metadata;
- original collaboration history;
- historical validation material;
- the repository state before professional modernization.

Later work must not rewrite or move this historical reference.

### Maintained Repository Baseline

The current `main` branch represents the actively maintained repository.

It may contain work performed after the historical baseline, including:

- documentation modernization;
- repository governance;
- continuous integration;
- quality tooling;
- API documentation;
- source-metadata cleanup;
- correctness fixes;
- later engineering improvements.

The maintained baseline therefore evolves over time.

A new commit on `main` does not alter what the historical 42 baseline
represents.

## Evolution Boundary

Future work should be classified before implementation.

The primary distinction is between:

1. preserving historical project context;
2. correcting demonstrated problems;
3. maintaining or improving the existing implementation;
4. extending the project beyond its original 42 scope;
5. performing experimental engineering work.

This classification helps reviewers understand why a change exists and
whether it changes runtime behaviour.

## Work Categories

### Correctness Fixes

Correctness fixes address demonstrated defects in existing behaviour.

Examples include:

- incorrect error propagation;
- descriptor leaks;
- invalid persistent shell state;
- memory-ownership defects;
- incorrect command status;
- behaviour that contradicts the maintained implementation contract.

Correctness fixes may change runtime behaviour because their purpose is to
replace incorrect behaviour with intended behaviour.

They should:

- originate from a focused GitHub issue;
- identify the demonstrated failure or violated invariant;
- avoid unrelated refactoring;
- include appropriate validation;
- update documentation when a maintained contract changes.

Known examples are tracked in #49 and #50.

### Maintenance and Reliability

Maintenance work improves robustness without introducing new shell
capabilities.

Examples may include:

- defensive cleanup;
- clearer resource ownership;
- improved failure recovery;
- removal of genuinely stale code;
- narrowly justified dependency cleanup;
- improved diagnostics.

Maintenance work should not be performed merely to make source code look
different from the historical implementation.

The expected benefit should outweigh source churn and regression risk.

### Testing and Validation

Testing work improves confidence in existing behaviour.

Possible directions include:

- automated behavioural regression tests;
- resource-oriented validation;
- additional parser and expansion cases;
- signal and process tests;
- leak and descriptor checks;
- coverage reporting where useful.

Historical 42 evaluation evidence remains historical evidence.

New automated validation should describe the maintained repository state
rather than retroactively redefining the original project evaluation.

### Tooling and Developer Experience

Tooling work improves repository maintenance without changing shell semantics.

Examples include:

- reproducible local validation commands;
- CI improvements;
- documentation generation;
- developer scripts;
- diagnostic tooling;
- repository-maintenance automation.

New tooling should have a demonstrated maintenance benefit and should avoid
unnecessary dependency growth.

### Architecture and Refactoring

Architecture work changes code structure rather than shell capability.

Examples may include:

- reducing problematic coupling;
- clarifying ownership boundaries;
- simplifying error propagation;
- isolating execution responsibilities;
- improving testability.

Refactoring should not be justified only by aesthetic preference.

Before broad structural changes, maintainers should identify:

- the concrete maintenance problem;
- the affected runtime contracts;
- regression risk;
- validation required;
- whether the change remains compatible with the project's historical context.

### Optional Post-42 Capabilities

Capabilities outside the original mandatory subject may be explored later.

Examples could include shell syntax or behaviour not implemented by the
preserved baseline.

Such work must be explicitly identified as post-42 development.

It must not be presented as functionality that existed in the original
evaluated project.

Optional capability work should normally be separated into focused issues or
larger explicitly scoped initiatives.

### Technical Experiments

Experiments may investigate alternative designs, tools or shell behaviour
without committing the repository to adoption.

Examples include:

- alternative parser strategies;
- stronger static-analysis configurations;
- experimental execution models;
- benchmarking;
- prototype shell capabilities.

Experimental work should remain clearly labelled and should not silently
become part of the maintained contract.

## Runtime-Behaviour Classification

Future changes should make their behavioural impact explicit.

| Category | Runtime behaviour may change? | Historical 42 claim? |
| --- | --- | --- |
| Documentation | No | No |
| Repository/tooling maintenance | Normally no | No |
| Correctness fix | Yes, narrowly | No |
| Reliability improvement | Possibly | No |
| Refactoring | Intended not to | No |
| Optional post-42 capability | Yes | Explicitly post-42 |
| Experiment | Possibly | Explicitly experimental |

A runtime-changing post-baseline commit does not change the preserved
historical baseline.

## Prioritization Model

Future work should be prioritized by evidence rather than novelty.

The primary factors are:

- correctness risk;
- persistence of the failure across shell commands;
- resource or memory impact;
- user-visible behaviour;
- maintainability benefit;
- regression risk;
- validation cost;
- implementation cost;
- portfolio value;
- relationship to the original 42 scope.

### P0: Release-Blocking Defect

Use P0 for a demonstrated issue severe enough that the maintained repository
should not be presented as a stable release candidate until it is resolved.

Examples could include:

- repeatable crashes in supported behaviour;
- persistent shell-state corruption;
- serious resource ownership failure;
- major incorrect execution semantics.

P0 should remain rare.

### P1: Demonstrated Correctness or Reliability Defect

Use P1 for a demonstrated defect that should be addressed before optional
enhancement work.

Examples include:

- narrow error-path correctness failures;
- descriptor leaks in the persistent shell process;
- missing allocation-error propagation;
- incorrect status or state handling.

Current issues #49 and #50 belong to this correctness queue.

Their exact implementation order may be chosen independently, but they should
normally precede optional post-42 feature work.

### P2: Engineering Improvement

Use P2 for justified improvements that increase maintainability, validation or
developer experience without correcting an urgent defect.

Examples include:

- broader automated regression testing;
- improved resource validation;
- targeted refactoring;
- useful tooling improvements.

### P3: Optional Exploration

Use P3 for work whose main value is experimentation, learning or portfolio
exploration.

Examples include:

- optional shell features;
- speculative architecture changes;
- experimental tooling;
- capabilities beyond the original subject.

P3 work is not automatically part of the repository roadmap merely because it
is technically possible.

## Known Correctness Follow-Ups

The code-quality audit identified two focused runtime defects.

### #49: Parent Redirection Recovery

Issue #49 tracks failure-path defects around parent-process redirection
handling and `dup2()` recovery.

The affected area includes:

- pending redirection descriptor ownership;
- standard-stream restoration;
- persistent parent-process state.

This is correctness work, not a post-42 feature.

Roadmap classification:

    Correctness Fix
    Priority: P1

### #50: Literal-Dollar Allocation Failure

Issue #50 tracks missing `ERR_MALLOC` propagation when allocation of a literal
dollar expansion fails.

This is a narrow error-propagation defect.

Roadmap classification:

    Correctness Fix
    Priority: P1

Neither fix is implemented by this roadmap workstream.

## Known Risks That Are Not Current Commitments

The code-quality audit also records findings that were not demonstrated as
runtime defects.

Examples include:

- direct Readline interaction from the interactive signal handler;
- partial environment-state consistency after post-`chdir()` failure;
- explicit `EINTR` handling around process waiting;
- transient dangling pointers during immediate failure unwind;
- the current `ft_memcpy()` analyzer precondition;
- small accepted duplication in wait-status helpers.

These findings remain documented evidence.

They do not automatically become committed implementation tasks.

A future issue should be created only when additional evidence or maintenance
value justifies work.

## Relationship to Current Limitations

The maintained README and architecture documentation explicitly describe
features that are not present in the current implementation.

Examples include optional shell grammar and heredoc-body variable expansion.

Documenting a missing capability does not create a requirement to implement
it.

If such functionality is added later, it should be identified explicitly as
post-baseline development and validated independently from the historical 42
project.

## Change Workflow

Post-baseline work should follow the maintained repository workflow:

    GitHub issue
          |
          v
    short-lived branch
          |
          v
    focused commits
          |
          v
    local validation
          |
          v
    pull request
          |
          v
    CI + review
          |
          v
    squash merge
          |
          v
    branch cleanup

Runtime-changing work should have a focused issue before implementation.

Large unrelated changes should not be combined merely because they were
discovered during the same audit or modernization activity.

## Documentation Expectations

A future change should update maintained documentation when it changes:

- architecture;
- ownership;
- API contracts;
- supported behaviour;
- testing expectations;
- CI or tooling guarantees;
- known limitations.

Historical documentation under `docs/history/` should not be rewritten to
describe later implementation state.

## Historical Separation

The historical boundary is preserved primarily through:

    portfolio-baseline-2026-08

Git history and the repository documentation provide the surrounding
development context.

Future work should not attempt to make historical files appear as though later
portfolio-oriented changes existed during the original 42 project.

Likewise, later improvements should not be hidden merely to make the current
repository resemble its original state.

The repository intentionally preserves both stories:

- what the original project was;
- how it was subsequently maintained and improved.

## Release Relationship

The maintained historical-baseline and release policy is defined in:

[`release-strategy.md`](release-strategy.md)

The release strategy preserves the distinction between:

    portfolio-baseline-2026-08

as an immutable historical reference and semantic-version maintained releases
such as:

    v1.0.0

The first maintained release is intentionally gated by unresolved P1
correctness work and the final portfolio audit.

This roadmap determines how future work is categorized and prioritized.

The release strategy determines when a maintained repository state is ready to
be published as a versioned release.

## Non-Commitment Principle

This roadmap is not a feature backlog.

A listed category, example or technical possibility does not mean that it must
be implemented.

Future work should be added only when there is a clear reason, explicit scope
and appropriate validation plan.

This keeps the repository understandable and prevents portfolio modernization
from turning into indefinite feature expansion.

## Current Direction

At the time this roadmap was established:

- the professional repository baseline is documented and maintained;
- the historical 42 state remains preserved;
- source metadata has been normalized without rewriting history;
- #49 and #50 are the current focused correctness follow-ups;
- automated behavioural regression testing remains a useful future engineering
  direction;
- optional post-42 capabilities remain non-committed;
- maintained release strategy is defined in `release-strategy.md`;
- final portfolio review remains a separate closing workstream.

The roadmap should be updated when the evolution policy itself changes, not
merely whenever an individual issue is opened or closed.
