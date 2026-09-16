# Development

This directory contains maintained technical documentation for the
post-academic Minishell repository.

It complements the project architecture, decision records and testing
documentation without reproducing the complete repository-maintenance process.

For contribution workflow and repository-maintenance conventions, see
[`CONTRIBUTING.md`](../../CONTRIBUTING.md).

## Maintained Documentation

### API Documentation

[`api-documentation.md`](api-documentation.md) defines the maintained
API-documentation boundary, documentation conventions and Doxygen strategy.

It explains which shared Minishell interfaces are documented and how generated
API documentation relates to the hand-written architecture documentation.

### Code Quality

[`code-quality.md`](code-quality.md) records the implementation-quality review,
including resource ownership, error paths, memory ownership, analyzer findings,
accepted trade-offs and known risks.

It is an engineering record of the maintained implementation rather than a
claim that the code is defect-free.

### Continuous Integration

[`continuous-integration.md`](continuous-integration.md) documents the
maintained GitHub Actions baseline, including reference and compiler-diversity
builds, dependency checks, no-relink validation, guarantees and limitations.

### Static Analysis and Quality Checks

[`static-analysis.md`](static-analysis.md) documents the quality tools evaluated
for this codebase and why compiler diversity is automated while broader static
analysis remains investigative.

## Repository State Model

The repository distinguishes three states:

| State | Meaning |
| --- | --- |
| `portfolio-baseline-2026-08` | Immutable repository state immediately before professional portfolio modernization |
| `main` | Current maintained repository state |
| `vMAJOR.MINOR.PATCH` | Deliberately selected immutable maintained release |

The historical baseline is not presented as the exact evaluated 42 commit
unless that relationship can be independently established.

Changes made after the baseline remain distinguishable from the original
academic project through Git history, documentation and release history.

Historical tags and published releases are immutable references and must not be
moved to follow later development.

## Contribution Workflow

The maintained workflow is documented at the repository root in
[`CONTRIBUTING.md`](../../CONTRIBUTING.md).

GitHub Issue Forms and the pull-request template under `.github/` provide the
structured entry points for current work.

The original two-person Jira-based workflow is historical material and remains
separate from current maintenance policy.

## Historical Project Material

Original and superseded project-era material is preserved under
[`../history/`](../history/).

That material records the original collaborative workflow, design artifacts,
planning and validation evidence without treating those practices as current
repository policy.

## Related Documentation

- [`../architecture/`](../architecture/) documents the maintained system
  architecture.
- [`../decisions/`](../decisions/) records significant engineering decisions.
- [`../testing/`](../testing/) documents current testing and validation.
- [`../history/`](../history/) preserves project-era historical material.
- [`../README.md`](../README.md) is the repository-wide documentation index.
