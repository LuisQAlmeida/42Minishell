# Development

This directory documents the current contribution, maintenance and repository
workflow for Minishell.

It describes how the repository is maintained after the original 42 project
baseline and should not be confused with the historical two-person team
workflow.

## Start Here

For contributors and maintainers beginning from the repository root:

- [`../../CONTRIBUTING.md`](../../CONTRIBUTING.md) provides the public
  contribution entry point;
- [`git-workflow.md`](git-workflow.md) documents the detailed Git and GitHub
  lifecycle;
- [`continuous-integration.md`](continuous-integration.md) documents the
  maintained GitHub Actions baseline, its guarantees and its limitations;
- [`static-analysis.md`](static-analysis.md) documents compiler-diversity
  quality checks, evaluated static-analysis tools and the tooling rationale.
- [`api-documentation.md`](api-documentation.md) defines the maintained
  API-documentation boundary, documentation conventions and Doxygen strategy.

## Current Workflow

The maintained development model is:

```text
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
```

The detailed workflow covers:

- GitHub Issues as the current work-tracking mechanism;
- branch naming and issue traceability;
- commit-message conventions;
- local validation;
- pull-request expectations;
- maintained GitHub Issue Forms and pull-request templates;
- CI and review;
- the maintained continuous-integration baseline;
- `CODEOWNERS`;
- protected `main` behaviour;
- squash merge;
- post-merge branch cleanup;
- synchronization of the local repository after merge.

See:

[`git-workflow.md`](git-workflow.md)

For the CI contract itself, see:

[`continuous-integration.md`](continuous-integration.md)

For compiler-quality and static-analysis decisions, see:

[`static-analysis.md`](static-analysis.md)

## Repository Governance

The maintained repository uses a protected `main` branch and short-lived
working branches.

Current governance and workflow policy are documented from the repository's
maintained configuration rather than inferred from historical project
agreements.

Where GitHub technically enforces a rule, the workflow identifies it as
repository-enforced policy.

Where a practice is maintained by convention rather than enforcement, it is
documented separately as maintained workflow.

This distinction is important for rules such as review expectations,
`CODEOWNERS` behaviour and approval requirements.

## Historical Workflow

The original Minishell project was developed by a two-person team using
additional Jira-based conventions and collaboration agreements.

Those records are preserved under:

[`../history/team-workflow/`](../history/team-workflow/)

Historical material includes:

- Jira-key policies;
- original branch and commit conventions;
- working agreements;
- definitions of ready and done;
- Git command references;
- original repository-governance notes.

These documents preserve project provenance.

They are not automatically authoritative for current portfolio maintenance.

## Related Documentation

- [`../README.md`](../README.md) defines the repository-wide documentation
  model.
- [`../architecture/`](../architecture/) documents the maintained system
  architecture.
- [`../decisions/`](../decisions/) records significant engineering decisions.
- [`../testing/`](../testing/) is the maintained testing and validation domain.
- [`../history/`](../history/) preserves original and superseded project
  material.
