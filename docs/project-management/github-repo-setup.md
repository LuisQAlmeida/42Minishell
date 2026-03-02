# GitHub Repository Setup (Professional Team Workflow) - 42 Minishell

This document records how this repository was configured for a professional 2-person team workflow (Luís & João).  
Goal: make collaboration safe, review-driven, and employer-readable (showing correct GitHub repo governance).

---

## Where to place this file in the repo

Put this file at:

- `docs/project-management/github-repo-setup.md`

Why: it is “project management / process” documentation (exactly what employers expect), and it keeps your README clean while still being discoverable.

---

## Why this setup exists

For real-world teams, the main risks are:
- someone accidentally pushing broken code to `main`
- merging without review
- merging code that does not build
- losing track of decisions and process

This setup is designed to:
- enforce Pull Request (PR) workflow
- require both teammates to review changes
- ensure CI checks pass before merging
- prevent force-push and accidental deletion of important branches

---

## Repository structure (process-related files)

This repo includes process files commonly used in professional teams:

### `.github/`
- `PULL_REQUEST_TEMPLATE.md`  
  Ensures PRs contain context, test instructions, and checklists.
- `CODEOWNERS`  
  Automatically requests reviews from required reviewers.
- `workflows/ci.yml`  
  Adds CI build checks to PRs (required before merge once enabled in branch rules).

### `docs/project-management/`
- `working-agreements.md`  
  Defines team workflow rules (branching, PRs, review expectations, DoD/DoR).
- `github-repo-setup.md` (this file)  
  Documents the GitHub governance setup for future reference and for employers.

---

## Collaboration model (recommended for 2-person Minishell)

### Long-lived branches
- `main` (protected): always stable, PR-only.

### Short-lived branches (one per Jira ticket / story)
Use:
- `feature/MSH-###-short-title`
- `fix/MSH-###-short-title`
- `refactor/MSH-###-short-title`
- `docs/MSH-###-short-title`
- `chore/MSH-###-short-title`
- `test/MSH-###-short-title`

This keeps work small, reviewable, and easy to trace.

---

## Required reviewers: Luís + João

### CODEOWNERS

Create `.github/CODEOWNERS`:

```txt
* @LuisQAlmeida @JOAO_GITHUB_USERNAME
```

Effect:
- every PR automatically requests review from both teammates
- supports enforcing “review from Code Owners” in branch protection rules

---

## Branch protection for `main`

Path:
**Repo → Settings → Branches → Branch protection rules → Add rule**

Branch name pattern:
- `main`

### Important note about GitHub plan

Some GitHub plans (notably Free) have limitations on enforcing certain protections for private repositories.  
If branch protections are not enforcing as expected, check:
- repository visibility (public vs private)
- current GitHub plan features

---

## Branch protection rule settings (what to tick and why)

This section documents the branch protection checkboxes and the reasoning behind each choice.

### ✅ Require a pull request before merging
**Why:** Prevents direct changes to `main`.  
All work must go through PR review, discussion, and CI checks.

### ✅ Require approvals (Required number of approvals: 2)
**Why:** With a 2-person team, this effectively requires **both Luís and João** to approve every merge.  
This is a strong signal of professional review discipline.

### ✅ Dismiss stale pull request approvals when new commits are pushed
**Why:** If commits are added after approval, GitHub invalidates the old approvals.  
Prevents “approved code” from being changed without reviewers noticing.

### ✅ Require review from Code Owners
**Why:** Ensures that code owners must approve changes.  
With CODEOWNERS set to both teammates, this reinforces “both must review.”

### ✅ Require status checks to pass before merging
**Why:** Prevents merging code that fails CI.  
At minimum, CI should confirm the project builds (e.g., `make`).

**Important:** You must select actual status checks once the first workflow run exists.  
If it shows “No required checks”, push a CI workflow, open a PR, let CI run, then come back to select the check (example: `CI / build`).

### ✅ Require branches to be up to date before merging
**Why:** Ensures PRs are tested against the latest `main`.  
Reduces “it passed on an old base” merges and limits surprise conflicts.

### ✅ Require conversation resolution before merging
**Why:** All review comment threads must be resolved first.  
Prevents merging while known issues are still open.

### ✅ Do not allow bypassing the above settings
**Why:** Prevents even admins from skipping the rules.  
This is a strong governance choice that signals mature process.

---

## Settings we intentionally did NOT enable (and why)

### ❌ Lock branch
**Why not:** Overkill for our use case. PR-only + protections are already sufficient.

### ❌ Allow force pushes
**Why not:** Force pushes rewrite history and can break collaboration and audit trails.

### ❌ Allow deletions
**Why not:** Prevents accidental deletion of the primary stable branch.

### Optional settings you may enable later
- **Require signed commits**  
  Great security practice, but adds setup overhead (GPG keys). Optional.
- **Require linear history**  
  Useful if you want strict rebase/squash-only history. Optional.
- **Require approval of the most recent reviewable push**  
  Adds extra safety but may add friction. Optional.

---

## CI (status checks) setup

A minimal GitHub Actions workflow is recommended so PRs always build before merge.

File:
`.github/workflows/ci.yml`

Minimal behavior:
- triggers on PRs
- runs `make` if a Makefile exists
- later can be extended (norm, tests, valgrind scripts)

Once CI runs at least once:
- go back to the branch protection rule
- select the CI check under “Require status checks to pass before merging”

---

## Expected day-to-day workflow

1. Create a Jira ticket (MSH-###)
2. Create a branch from `main` (example: `feature/MSH-21-tokenizer-operators`)
3. Commit small, focused changes
4. Push branch and open PR into `main`
5. CI runs automatically
6. Both teammates review and approve
7. Merge PR only after:
   - approvals (2)
   - CI checks pass
   - conversations resolved
8. Delete feature branch (optional but recommended)

---

## Outcome

This repo is configured to:
- enforce PR-based collaboration
- enforce 2-person review discipline
- prevent unsafe history edits (force pushes)
- ensure code quality gates (CI) before merge
- document the process for future reference and employer visibility
