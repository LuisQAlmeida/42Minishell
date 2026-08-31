# Historical Baseline and Maintained Release Strategy

This document defines how historical Git references, maintained versions and
GitHub Releases are used in the Minishell repository.

The repository intentionally preserves two different stories:

- the original 42 project as it existed before professional modernization;
- the later maintained repository used as a software-engineering portfolio.

Those histories must remain distinguishable.

## Reference Model

The repository uses three different concepts.

### Historical Baseline

The immutable historical reference is:

    portfolio-baseline-2026-08

It identifies the repository state immediately before professional portfolio
modernization.

### Maintained Branch

The `main` branch represents the current maintained repository state.

It evolves as issues and pull requests are completed.

### Maintained Release

A maintained release is a deliberately selected, validated state of `main`
published through a versioned annotated Git tag and corresponding GitHub
Release.

A maintained release is therefore different from both the historical baseline
and an arbitrary commit on `main`.

## Historical Baseline

The annotated tag:

    portfolio-baseline-2026-08

resolves to:

    7ba2005223e263964bf8ae3069da9ff54c2c2c2b

The referenced commit is:

    7ba2005 Final tests and checks (#25)

This tag preserves the project state before professional portfolio
modernization.

It is an historical reference, not a maintained software release.

### Historical-Baseline Invariants

`portfolio-baseline-2026-08` must:

- remain immutable;
- continue to point to the same historical commit;
- never be moved to follow `main`;
- never be reused as a maintained version;
- never be renamed to imitate a semantic-version release;
- remain available for comparison with later maintained development.

The tag may be documented more fully in the future, but its target must not
change.

### What the Historical Baseline Represents

The historical baseline preserves evidence of:

- the original Minishell implementation;
- the original collaborative development history;
- project-era repository state;
- historical source metadata;
- original evaluation and validation context.

Later documentation, CI, quality tooling, maintenance work and runtime fixes
must not be presented as though they existed in that historical state.

## Maintained Main Branch

`main` is the repository's evolving maintained baseline.

A merge into `main` establishes a new maintained repository state, but does not
automatically create a release.

This distinction is intentional.

Examples of work that may exist on `main` between releases include:

- documentation improvements;
- repository-maintenance changes;
- correctness fixes;
- CI and tooling improvements;
- refactoring;
- optional post-42 development.

Not every merged pull request requires a new release.

## Maintained Version Tags

Maintained software releases use conventional semantic-version-style tags:

    vMAJOR.MINOR.PATCH

Examples:

    v1.0.0
    v1.0.1
    v1.1.0
    v2.0.0

The `v` prefix distinguishes maintained software versions clearly from the
historical tag:

    portfolio-baseline-2026-08

This familiar format is preferred over repository-specific names such as
`portfolio-v1` or date-only maintained releases.

## Semantic-Version Interpretation

The repository uses Semantic Versioning concepts as a practical release
communication model.

Minishell is not a packaged library with a formally versioned public API, so
version numbers describe the maintained project contract rather than claiming
strict library ABI compatibility.

The relevant contract includes:

- supported command behaviour;
- documented shell semantics;
- maintained user-visible capabilities;
- repository expectations that materially affect use of the project.

### PATCH

Increment PATCH for release-worthy compatible maintenance that does not add a
new supported capability or intentionally break existing maintained behaviour.

Typical examples include:

- correctness fixes;
- narrow reliability fixes;
- error-path fixes;
- resource-ownership fixes;
- documentation corrections shipped with maintained fixes;
- tooling or maintenance updates included in a patch release.

Example:

    v1.0.0 -> v1.0.1

Documentation-only or tooling-only changes do not require an immediate release
by themselves.

If they are included in a later release, the version is determined by the
highest-impact change included in that release.

### MINOR

Increment MINOR when the maintained project gains a new backwards-compatible
capability or substantial compatible functionality.

Typical examples include:

- a new supported shell feature;
- a significant compatible post-42 capability;
- substantial new validation or tooling functionality when it materially
  changes the maintained project offering.

Example:

    v1.0.1 -> v1.1.0

Any capability beyond the original 42 subject must remain explicitly described
as post-42 development.

### MAJOR

Increment MAJOR when a maintained release intentionally introduces an
incompatible change to an established supported contract.

Examples could include:

- deliberately incompatible shell behaviour;
- removal of previously maintained supported behaviour;
- a major interface or usage-contract redesign.

Example:

    v1.4.2 -> v2.0.0

A large internal refactor alone does not require a major-version increment if
the maintained external behaviour remains compatible.

## Pre-1.0 Policy

The repository does not need to create provisional `v0.x` releases merely
because modernization work exists on `main`.

The intended first maintained portfolio release is:

    v1.0.0

That version should represent a consciously validated professional repository
baseline rather than an intermediate modernization checkpoint.

Until its release-readiness gates pass, `main` remains the maintained
development baseline without a semantic-version release tag.

## Release Readiness

A stable maintained release must be created deliberately.

At minimum, the candidate commit should satisfy the following gates.

### Repository State

- the release candidate is on protected `main`;
- local `main` and `origin/main` resolve to the intended commit;
- the working tree is clean;
- the release candidate was integrated through the maintained pull-request
  workflow;
- no temporary release branch is required after the candidate is on `main`.

### Build and CI

- `CI / build` passes for the candidate state;
- `CI / quality` passes for the candidate state;
- the reference `make` build succeeds;
- the supplemental `make CC=clang` build succeeds;
- compiler warnings remain within the maintained zero-warning build baseline.

### Documentation

- maintained documentation reflects current behaviour;
- architecture documentation remains consistent with the implementation;
- known limitations are documented;
- the evolution roadmap reflects the current policy;
- release notes can distinguish historical, maintained and post-42 work.

### Repository Hygiene

- generated documentation is not versioned;
- analyzer build artefacts are not versioned;
- `git diff --check` passes for the final release work;
- historical references remain unchanged.

### Quality Findings

P0 findings block a stable maintained release.

P1 correctness findings should also normally be resolved before a stable
maintained release.

If a future release intentionally ships with an unresolved P1 finding, that
must be an explicit documented release decision rather than an accidental
omission.

The first maintained `v1.0.0` release has a stricter rule:

    no known open P0 or P1 correctness issue

There is no exception to that rule for the first maintained release.

## Current P1 Release Gates

The evolution roadmap identified two P1 correctness issues. Both have now been
resolved.

### #49 Parent Redirection Recovery

Issue #49 resolves the demonstrated parent-process redirection failure-path
defects involving descriptor ownership and standard-stream restoration.

Release policy:

    RESOLVED FOR v1.0.0

### #50 Literal-Dollar Allocation Failure

Issue #50 resolves missing allocation-error propagation in the literal-dollar
expansion path.

Release policy:

    RESOLVED FOR v1.0.0

Both identified P1 correctness gates have now been satisfied. The final
portfolio audit prerequisite has also been completed.

## Final Portfolio Audit Gate

The final portfolio audit is also a prerequisite for `v1.0.0`.

That prerequisite has now been satisfied.

Its purpose is different from the correctness gates.

The P1 issues answer:

    Are known demonstrated correctness defects still open?

The final portfolio audit answers:

    Is the repository as a whole internally consistent, validated and ready to
    be presented as the maintained portfolio release?

Therefore the first maintained release sequence is:

    define release strategy
            |
            v
    resolve required P1 issues
            |
            v
    final portfolio audit
            |
            v
    validate release candidate on main
            |
            v
    reconcile release-state documentation
            |
            v
    integrate release preparation through PR + CI
            |
            v
    validate exact final main state
            |
            v
    create annotated v1.0.0 tag
            |
            v
    publish matching GitHub Release

## Annotated Git Tags

Maintained release tags should be annotated Git tags.

Example:

    git tag -a v1.0.0 -m "Minishell maintained release v1.0.0"

Annotated tags are preferred because they preserve explicit tag metadata and a
human-readable release annotation.

A maintained release tag must point to the exact `main` commit selected as the
release candidate.

Release tags must not be moved after publication.

If a released state later requires correction, create a new version rather
than retargeting the existing tag.

For example:

    v1.0.0
        |
        +-- defect discovered
                |
                v
             v1.0.1

## GitHub Releases

Each stable maintained semantic-version tag should normally have a matching
GitHub Release.

The relationship is:

    validated main commit
            |
            v
    annotated Git tag
            |
            v
    GitHub Release for the same tag

The Git tag is the versioned Git reference.

The GitHub Release is the human-facing publication layer containing release
context and notes.

A GitHub Release must not point to a different source state from its named
version tag.

## Minimum Release Notes

A maintained GitHub Release should contain enough information for a reviewer
to understand what the release represents.

At minimum, include:

- release purpose;
- major changes since the previous maintained release;
- validation performed;
- known limitations;
- relevant correctness fixes;
- any optional post-42 capabilities included;
- a reference to the historical baseline where useful.

For `v1.0.0`, the release notes should explicitly explain that:

- it is the first professionally maintained portfolio release;
- it is not the original 42 evaluation state;
- the original pre-modernization state remains preserved at
  `portfolio-baseline-2026-08`;
- later modernization and correctness work is intentionally distinguishable
  through Git history.

## Historical Baseline and GitHub Releases

`portfolio-baseline-2026-08` does not need to be converted into a conventional
GitHub software release.

Its primary purpose is historical comparison and provenance.

If GitHub UI documentation is ever added for that tag, it must remain clearly
labelled as an historical baseline rather than `v1.0.0` or another maintained
software release.

## Release Procedure

When a maintained release candidate is ready:

1. confirm all release-readiness gates;
2. confirm the intended commit is on `main`;
3. confirm local `main` and `origin/main` match;
4. confirm the working tree is clean;
5. run the documented local validation;
6. confirm required GitHub Actions checks are green;
7. verify known P0/P1 findings against the release policy;
8. verify the historical baseline still resolves to its immutable commit;
9. create the annotated semantic-version tag;
10. push that tag;
11. create the matching GitHub Release;
12. verify the GitHub Release resolves to the same tag and commit.

The exact terminal commands should be chosen at release time after the final
candidate commit is known.

This document deliberately does not hard-code a future release commit hash.

## Post-Release Development

After a maintained release, normal issue-driven development continues from
`main`.

A released version remains immutable.

Later work should result in:

- no release when a merged change does not justify publication;
- a PATCH release for compatible correctness or maintenance work;
- a MINOR release for new compatible maintained capabilities;
- a MAJOR release for intentional incompatible maintained-contract changes.

The version decision should reflect the highest-impact change since the
previous maintained release.

## Optional Post-42 Development

Optional capabilities beyond the original 42 subject may be included in future
maintained releases.

When they are included:

- they must be labelled as post-42 work;
- release notes must not imply they existed in the historical project;
- historical documentation must not be rewritten to include them;
- the version increment should reflect their maintained behavioural impact.

The historical baseline remains unchanged regardless of later capabilities.

## Relationship to the Evolution Roadmap

The evolution roadmap defines:

- how future work is categorized;
- how P0-P3 priorities are interpreted;
- how historical and maintained development remain separate.

This release strategy defines:

- when a maintained state becomes a release;
- how that release is named;
- which quality gates apply;
- how Git tags and GitHub Releases represent it.

The two documents are complementary.

## Non-Goals

This release strategy does not:

- implement #49;
- implement #50;
- create `v1.0.0`;
- create any GitHub Release;
- modify `portfolio-baseline-2026-08`;
- turn every `main` commit into a release;
- require every documentation change to increment a version;
- define speculative post-42 features.

## Current Release State

At the current maintained state:

- `portfolio-baseline-2026-08` is the only existing tag;
- it is an annotated historical tag;
- no semantic-version maintained release exists;
- `main` is the current maintained development baseline;
- #49 has resolved its P1 correctness gate;
- #50 has resolved its P1 correctness gate;
- no identified P1 correctness release gate remains open;
- the final portfolio audit has been completed;
- release-candidate validation completed successfully against maintained
  `main` commit `4c036f5`;
- release publication preparation is tracked through #62;
- `v1.0.0` has not yet been created;
- the exact final `main` state must still be validated after the release
  preparation changes are integrated and before the tag is created.

The first maintained portfolio release becomes eligible only after its
documented readiness gates pass against the exact state selected for tagging.
