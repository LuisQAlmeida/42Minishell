# Contributing

This repository preserves a completed 42 Minishell project while continuing to
evolve as a professional software-engineering portfolio.

Contributions and maintenance changes should preserve that distinction.

## Start with an Issue

Meaningful repository changes should begin with a GitHub issue.

The issue should describe:

- the problem or improvement;
- the intended scope;
- relevant constraints;
- acceptance criteria where useful.

This keeps later branches, commits and pull requests connected to a clear unit
of work.

Small corrections that do not justify separate tracking may be handled
directly in a focused maintenance branch.

## Development Workflow

The maintained repository uses short-lived branches and pull requests rather
than direct development on `main`.

The normal lifecycle is:

```text
issue
  |
  v
short-lived branch
  |
  v
focused commits
  |
  v
pull request
  |
  v
validation and review
  |
  v
squash merge
  |
  v
branch cleanup
```

Detailed branch, commit, validation, pull-request and merge conventions are
documented in:

[`docs/development/git-workflow.md`](docs/development/git-workflow.md)

## Branches

Create maintenance branches from an up-to-date `main`.

Use:

```text
<type>/<issue-number>-<short-description>
```

Common branch types include:

- `feat/` for new functionality;
- `fix/` for bug fixes;
- `docs/` for documentation;
- `test/` for testing work;
- `refactor/` for behaviour-preserving restructuring;
- `ci/` for automation and CI changes;
- `chore/` for repository maintenance.

For example:

```text
docs/35-contribution-git-workflow
fix/42-heredoc-interrupt-cleanup
test/56-signal-regressions
```

The issue number provides repository-level traceability without depending on
the Jira conventions used during the original project.

## Commits

Prefer small, coherent commits that describe one understandable change.

Current portfolio-maintenance commits use a lightweight conventional-style
format:

```text
<type>(<scope>): <summary>
```

For example:

```text
docs(repo): standardize metadata and README
docs(architecture): document current architecture and runtime flow
docs(decisions): establish ADR convention and index
```

A scope may be omitted when it does not add useful information.

Working commits on a feature branch do not need to correspond one-to-one with
the final history of `main`.

Pull requests are squash-merged, so several focused branch commits normally
become one durable commit on `main`.

## Pull Requests

Open pull requests against `main`.

A pull request should:

- have one clear purpose;
- remain within the scope of its issue;
- explain what changed and why;
- describe relevant validation;
- avoid unrelated cleanup;
- resolve review conversations before merge.

The repository runs a CI build workflow for pull requests. Contributors should
ensure that applicable validation passes before merge.

The repository's GitHub configuration and pull-request templates may evolve
through separate modernization workstreams. The detailed development guide
documents the maintained workflow independently of historical template text.

## Merge Policy

`main` is the maintained stable branch.

Changes to `main` are integrated through pull requests and squash merge.

This allows a working branch to contain several useful intermediate commits
while keeping the long-lived branch concise:

```text
feature branch

commit A
commit B
commit C
    |
    v
pull request
    |
    v
squash merge
    |
    v
main

one durable commit
```

After merge, delete the short-lived branch and synchronize the local `main`
with the remote repository.

## Historical Project Work

The original Minishell project was developed collaboratively during the
42 Porto Common Core.

Its Git history and contributor attribution are intentionally preserved.

Historical Jira conventions, team agreements and development procedures remain
available under:

[`docs/history/team-workflow/`](docs/history/team-workflow/)

They provide project provenance but are not automatically current contribution
policy.

Portfolio maintenance should not rewrite historical commits, remove original
authorship or present later work as part of the original 42 delivery.

## Documentation

The maintained documentation entry point is:

[`docs/`](docs/)

Current contribution and repository-maintenance documentation lives under:

[`docs/development/`](docs/development/)

The source code remains authoritative for implemented behaviour, while current
documentation describes the maintained repository and historical material
preserves project provenance.
