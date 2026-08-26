# Git Workflow

This document defines the maintained Git and GitHub workflow for repository
maintenance and future development.

It describes the current portfolio-maintenance process rather than the workflow
used during the original two-person 42 project.

For the public contribution entry point, see
[`../../CONTRIBUTING.md`](../../CONTRIBUTING.md).

## Workflow at a Glance

The normal lifecycle for meaningful work is:

```text
identify work
      |
      v
GitHub issue
      |
      v
synchronize main
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
push branch
      |
      v
pull request into main
      |
      +--> CI build
      +--> review / discussion
      +--> resolve conversations
      |
      v
squash merge
      |
      v
delete branch
      |
      v
synchronize local main
```

The workflow is intentionally lightweight.

It provides traceability and a clean long-lived history without reproducing the
heavier Jira-based process used during the original project.

## Policy Levels

Not every workflow practice has the same enforcement mechanism.

This documentation distinguishes three categories.

### Repository-enforced policy

These behaviours are enforced by the current GitHub repository configuration:

- changes to protected `main` go through a pull request;
- `main` uses linear history;
- pull-request conversations must be resolved before merge;
- force pushes to `main` are blocked;
- deletion of `main` is restricted;
- squash merge is the enabled merge strategy for maintained pull requests.

The current protection configuration does not require an approval count greater
than zero.

### Maintained workflow

These practices are part of the current repository-maintenance process even
when GitHub does not technically enforce every step:

- begin meaningful work from a GitHub issue;
- branch from an up-to-date `main`;
- include the issue number in the branch name;
- create focused, understandable commits;
- validate the change before opening or merging a pull request;
- allow the pull-request CI build to complete successfully;
- review the final diff before merge;
- delete short-lived branches after merge;
- synchronize the local `main` after the merge.

These conventions should be treated as the normal development contract for the
repository.

### Historical workflow

The original project used additional collaboration conventions including:

- Jira issue keys such as `MSH-21`;
- Jira-key branch and commit naming;
- two-person working agreements;
- stronger teammate-approval expectations;
- project-specific definitions of ready and done.

Those records remain under
[`../history/team-workflow/`](../history/team-workflow/).

They preserve project provenance but are not current maintenance policy unless
a rule is explicitly re-established in maintained documentation.

## 1. Start from an Issue

Meaningful changes should normally begin with a GitHub issue.

An issue gives the work a durable identity before implementation begins.

Useful issue content includes:

- context;
- objective;
- intended scope;
- explicit exclusions;
- acceptance criteria;
- dependencies or parent initiatives when relevant.

The issue becomes the traceability anchor for the branch and pull request.

For example:

```text
Issue #35
Establish contribution and Git workflow
```

may be implemented on:

```text
docs/35-contribution-git-workflow
```

Small corrections that do not justify separate tracking may still use a
focused maintenance branch without a dedicated issue.

### Current issue templates

The repository provides maintained GitHub Issue Forms under
[`.github/ISSUE_TEMPLATE/`](../../.github/ISSUE_TEMPLATE/).

The current forms are:

| Form | Intended use |
| --- | --- |
| [`bug-report.yml`](../../.github/ISSUE_TEMPLATE/bug-report.yml) | Incorrect or unexpected maintained behaviour |
| [`enhancement.yml`](../../.github/ISSUE_TEMPLATE/enhancement.yml) | New capability or deliberate improvement |
| [`maintenance.yml`](../../.github/ISSUE_TEMPLATE/maintenance.yml) | Documentation, testing, refactoring, CI or repository maintenance |

The forms collect structured engineering context without requiring historical
Jira terminology.

They intentionally do not assign labels or assignees automatically.

The issue chooser also allows blank issues through
[`config.yml`](../../.github/ISSUE_TEMPLATE/config.yml), so work that does not
fit the maintained forms can still be recorded without forcing it into the
wrong category.

Issue Forms support the workflow; they do not replace the need to keep an issue
focused and give it a clear objective and scope.

## 2. Synchronize `main`

Before creating a branch, synchronize the local stable branch with the remote:

```bash
git switch main
git pull --ff-only origin main
git fetch origin --prune
git status
```

`git pull --ff-only` is preferred here because synchronization should not create
an accidental merge commit on `main`.

The expected state is:

```text
On branch main
Your branch is up to date with 'origin/main'.

nothing to commit, working tree clean
```

Do not start unrelated work from a dirty working tree.

## 3. Create a Short-Lived Branch

Use:

```text
<type>/<issue-number>-<short-description>
```

Common branch types are:

| Type | Purpose |
| --- | --- |
| `feat/` | New functionality |
| `fix/` | Bug fixes |
| `docs/` | Documentation |
| `test/` | Testing or validation work |
| `refactor/` | Behaviour-preserving code restructuring |
| `ci/` | Continuous-integration or automation work |
| `chore/` | Repository maintenance |

Examples:

```text
docs/35-contribution-git-workflow
fix/42-heredoc-interrupt-cleanup
test/56-signal-regressions
ci/60-static-analysis
```

Create and publish the branch with:

```bash
git switch -c docs/35-contribution-git-workflow
git push -u origin docs/35-contribution-git-workflow
```

The GitHub issue number replaces the Jira-key dependency used by the historical
workflow.

Keep branch names short enough to scan quickly while still making their purpose
clear.

## 4. Keep the Change Focused

A branch should represent one coherent piece of work.

Avoid mixing unrelated changes such as:

```text
documentation rewrite
+
unrelated source refactor
+
formatting cleanup
+
CI changes
```

unless the issue explicitly requires all of them.

A focused branch makes the pull request easier to understand, validate and
review.

If unrelated work is discovered, prefer recording it as a separate issue
instead of silently expanding the active branch.

## 5. Make Focused Commits

Working commits should describe understandable increments of the change.

The maintained repository uses a lightweight conventional-style format:

```text
<type>(<scope>): <summary>
```

Examples:

```text
docs(repo): standardize metadata and README
docs(architecture): document current architecture and runtime flow
docs(decisions): establish ADR convention and index
fix(exec): preserve pipeline exit status
test(signals): cover heredoc interruption
```

A scope may be omitted when it adds no useful information:

```text
docs: fix broken development link
```

Commit messages should:

- use an imperative or concise action-oriented summary;
- identify the kind of change;
- avoid vague messages such as `update`, `changes` or `fix stuff`;
- describe the commit itself rather than the entire issue.

The GitHub issue number does not need to be repeated in every commit because the
branch and pull request provide issue-level traceability.

### Working history versus `main` history

Several focused commits may exist on the feature branch:

```text
branch

commit A
commit B
commit C
commit D
```

That is useful working history.

The pull request is later squash-merged:

```text
commit A
commit B
commit C
commit D
    |
    v
pull request
    |
    v
squash
    |
    v
one commit on main
```

This allows detailed iteration during development while keeping `main`
portfolio-readable.

## 6. Validate Before the Pull Request

Validation should match the kind of change being made.

At minimum, inspect the working tree and patch:

```bash
git status
git diff --check
git diff
```

For staged work:

```bash
git diff --cached --check
git diff --cached
```

For source changes, applicable validation may include:

```bash
make
```

and relevant manual or regression tests.

Depending on the subsystem, additional checks may include:

- Norminette where applicable to 42-style source files;
- Valgrind for memory-sensitive paths;
- file-descriptor checks for pipes and redirections;
- exit-status validation;
- signal-behaviour validation;
- documentation link or formatting checks.

Validation requirements should be proportional to the change rather than
performed mechanically when they are irrelevant.

Documentation-only changes should not pretend to have exercised runtime
behaviour they did not modify.

## 7. Push the Branch

After committing:

```bash
git push
```

For the first push of a branch without an upstream:

```bash
git push -u origin <branch-name>
```

Confirm afterward:

```bash
git status
```

The local branch should report that it is up to date with its remote tracking
branch.

## 8. Open a Pull Request

Open the pull request against:

```text
base: main
```

The pull request should clearly explain:

- what changed;
- why the change exists;
- what was deliberately kept out of scope;
- how the work was validated;
- which issue it resolves.

When the pull request should automatically close its GitHub issue after merge,
use a supported closing reference such as:

```text
Closes #35
```

Keep the pull request focused on one primary unit of work.

### Current pull-request template

The repository provides a maintained pull-request template at:

[`.github/PULL_REQUEST_TEMPLATE.md`](../../.github/PULL_REQUEST_TEMPLATE.md)

The template follows the current GitHub Issues workflow and captures:

- the linked issue;
- a concise summary;
- motivation;
- included and excluded scope;
- validation actually performed;
- review notes when useful;
- a final merge checklist.

It deliberately avoids historical Jira `MSH-*` keys, Mandatory / Bonus
classification and teammate-specific approval language.

Validation remains proportional to the change. A documentation-only pull
request, for example, should not claim runtime testing that was not performed.

The template supports consistent pull requests, while this document remains
the authoritative description of the full repository workflow.

## 9. CI and Validation on GitHub

The repository currently defines:

[`../../.github/workflows/ci.yml`](../../.github/workflows/ci.yml)

The workflow runs:

- for pull requests;
- for pushes to `main`.

Its current job installs the readline development dependency and builds the
project when a `Makefile` exists.

A successful build is useful evidence that the pull request still compiles.

The current CI workflow should not be described as a complete quality gate.

Automated tests, static analysis and broader quality checks belong to separate
modernization workstreams.

Before merge, confirm that the available checks have completed successfully.

## 10. Review and `CODEOWNERS`

The repository currently defines:

[`../../.github/CODEOWNERS`](../../.github/CODEOWNERS)

with repository-wide ownership entries for the original collaborators.

This preserves ownership context and can cause GitHub to request appropriate
reviewers.

`CODEOWNERS` should not be confused with the current approval requirement.

The repository currently permits pull-request merge without requiring a
positive approval count, although unresolved review conversations must be
resolved before merge.

Review remains useful even when an approval is not technically required.

Relevant review may examine:

- scope;
- correctness;
- unintended behaviour changes;
- resource ownership;
- documentation accuracy;
- test evidence;
- historical attribution;
- consistency with accepted ADRs.

## 11. Keep the Branch Current When Necessary

A branch does not need continual synchronization with `main` merely for visual
tidiness.

Update it when:

- `main` has changed in a way relevant to the active work;
- GitHub reports a conflict;
- validation against the newer base is necessary;
- repository policy requires an updated branch before merge.

Before integrating changes from `main`, first fetch the current remote state:

```bash
git fetch origin
```

Because the maintained repository values linear long-lived history, avoid
introducing unnecessary merge commits purely to refresh a feature branch.

The exact conflict-resolution method should be chosen deliberately based on
whether the branch is private or shared and whether rewriting its history is
safe.

Do not force-push shared history casually.

## 12. Review the Final Pull Request Diff

Before merging, inspect the pull request as a whole rather than relying only on
individual working commits.

Confirm:

- the changed files match the issue scope;
- no temporary or generated files were included;
- no unrelated source changes slipped into documentation work;
- documentation links remain valid;
- validation results match the claims in the pull request;
- review conversations are resolved;
- the final commit message produced by squash merge will be meaningful on
  `main`.

A useful local scope check is:

```bash
git fetch origin
git diff --name-status origin/main...HEAD
git diff --check origin/main...HEAD
```

## 13. Squash Merge

The maintained repository uses squash merge for pull requests.

The branch may contain:

```text
commit A
commit B
commit C
```

but `main` receives one durable commit representing the completed unit of work.

The squash commit should summarize the completed change rather than merely copy
an intermediate branch commit.

For example:

```text
docs(development): establish contribution and Git workflow (#NN)
```

This keeps the long-lived history concise while the pull request preserves the
detailed development conversation and branch-level work.

## 14. Delete the Merged Branch

After a successful merge, the short-lived remote branch should be removed.

The repository is configured to clean up merged head branches automatically
through GitHub.

Synchronize locally:

```bash
git switch main
git pull --ff-only origin main
git fetch origin --prune
```

Then delete the local feature branch.

For a normal merged branch:

```bash
git branch -d <branch-name>
```

After a squash merge, Git may not recognize the feature branch tip as an
ancestor of `main` because the branch commits were replaced by a new squash
commit.

Once the merge has been independently confirmed, local cleanup may therefore
require:

```bash
git branch -D <branch-name>
```

Use `-D` only after confirming that the pull request was successfully merged
and the branch contains no work that still needs to be preserved.

## 15. Confirm the New Baseline

After cleanup:

```bash
git status
git log -5 --oneline
git branch -vv
```

The expected state is:

```text
main
  |
  `--> synchronized with origin/main

working tree
  |
  `--> clean

feature branch
  |
  `--> removed
```

The squash commit on `main` becomes the new baseline for the next workstream.

## Repository Governance Summary

The current maintenance model can be summarized as:

| Concern | Current model |
| --- | --- |
| Stable branch | `main` |
| Work tracking | GitHub Issues |
| Working branches | Short-lived, issue-linked branches |
| Direct development on `main` | Avoided through protected PR workflow |
| Branch naming | `<type>/<issue-number>-<short-description>` |
| Working commits | Focused conventional-style messages |
| Pull requests | Target `main` |
| Automated check | CI build |
| Required approval count | 0 |
| Review conversations | Must be resolved before merge |
| Long-lived history | Linear |
| Merge strategy | Squash |
| Force pushes to `main` | Blocked |
| Deletion of `main` | Restricted |
| Merged remote branches | Automatically cleaned up |
| Historical Jira workflow | Preserved, not current policy |

## Historical Provenance

The original two-person workflow is preserved under:

[`../history/team-workflow/`](../history/team-workflow/)

That material includes historical:

- Jira conventions;
- branch and commit policies;
- Git commands;
- definitions of ready and done;
- working agreements;
- GitHub repository setup notes.

It should be consulted when studying the original project process, not used
silently as the current repository workflow.

In particular, current maintenance must not imply that:

- Jira remains the active work tracker;
- every branch requires an `MSH-*` key;
- every commit requires a Jira key;
- two approvals are currently required;
- the original collaborators are still operating under the historical team
  agreement.

The repository preserves the historical record rather than rewriting it to
match later portfolio maintenance.

## Related Documentation

- [`../../CONTRIBUTING.md`](../../CONTRIBUTING.md) provides the public
  contribution entry point.
- [`README.md`](README.md) indexes current development documentation.
- [`../README.md`](../README.md) defines the repository-wide documentation
  model.
- [`../decisions/README.md`](../decisions/README.md) documents engineering
  decision records.
- [`../history/team-workflow/`](../history/team-workflow/) preserves the
  original project workflow.
