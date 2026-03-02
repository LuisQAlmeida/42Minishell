# Git Commands for Our Team Workflow (42 Minishell)

This is a practical command reference aligned with our repo rules:
- short-lived feature branches
- PR-only merges to `main` (once branch protection is enabled)
- clean, reviewable history

---

## 0) Quick context: our branch naming

Examples:
- `feature/MSH-21-tokenizer-operators`
- `fix/MSH-42-fd-leak-pipeline`
- `docs/MSH-10-architecture`
- `chore/MSH-01-repo-setup`

---

## 1) First-time repo setup (one-time)

```bash
# Clone the repo
git clone <REPO_URL>
cd <REPO_FOLDER>

# Confirm you’re on main and up-to-date
git checkout main
git pull
```

---

## 2) Daily sync: update your local `main`

Do this before starting work:

```bash
git checkout main
git pull
```

---

## 3) Create a new branch for a ticket/story

```bash
git checkout main
git pull
git checkout -b feature/MSH-123-short-title
```

---

## 4) Check what changed

```bash
git status
git diff
git diff --staged
```

---

## 5) Stage and commit changes

### Stage files
```bash
git add <file>
# or stage everything
git add .
```

### Commit (use our convention)
```bash
git commit -m "MSH-123 feat: short description"
# examples:
# git commit -m "MSH-21 feat: tokenize operators"
# git commit -m "MSH-42 fix: close unused pipe ends"
# git commit -m "MSH-10 docs: add architecture overview"
```

---

## 6) Push your branch and set upstream (first push)

```bash
git push -u origin feature/MSH-123-short-title
```

After the first push, you can just use:

```bash
git push
```

---

## 7) Open a Pull Request (PR)

From GitHub:
- open PR from your branch into `main`
- fill PR template (What/Why/How to test)
- request review (CODEOWNERS should auto-request)

> Once branch protection is enabled, merging will require approvals + CI checks.

---

## 8) Keep your branch up to date with `main`

If GitHub requires “branch up to date”, do:

```bash
git checkout main
git pull
git checkout feature/MSH-123-short-title
git merge main
```

Resolve conflicts if any, then:

```bash
git add .
git commit -m "MSH-123 chore: resolve merge conflicts with main"
git push
```

Alternative (cleaner history, but more advanced):
```bash
git rebase main
# resolve conflicts, then:
git push --force-with-lease
```

Only use `rebase` + force push if both teammates agree and you know what you’re doing.

---

## 9) After PR is merged: cleanup local and remote branches

### Switch away from the feature branch
```bash
git checkout main
git pull
```

### Delete local branch
```bash
git branch -d feature/MSH-123-short-title
```

### Delete remote branch (optional; GitHub UI often offers this)
```bash
git push origin --delete feature/MSH-123-short-title
```

---

## 10) Undo / recovery (common cases)

### A) Unstage a file (keep changes)
```bash
git restore --staged <file>
```

### B) Discard local changes in a file (dangerous)
```bash
git restore <file>
```

### C) Amend the last commit message or add missing file
```bash
git add <missing-file>
git commit --amend
# if already pushed:
git push --force-with-lease
```

### D) Revert a commit (safe for shared history)
```bash
git revert <commit_hash>
```

### E) Reset last commit locally (not pushed yet)
```bash
git reset --soft HEAD~1   # keep changes staged
# or
git reset --mixed HEAD~1  # keep changes unstaged
# or
git reset --hard HEAD~1   # discard changes (danger)
```

---

## 11) Inspect history and find changes

```bash
git log --oneline --graph --decorate --all
git show <commit_hash>
git blame <file>
```

---

## 12) Remote troubleshooting

### Check remote URLs
```bash
git remote -v
```

### Set upstream if missing
```bash
git push --set-upstream origin <branch_name>
```

---

## 13) Useful quality-of-life settings (optional)

### Auto-set upstream on first push
```bash
git config --global push.autoSetupRemote true
```

### Make `git status` shorter & nicer
```bash
git config --global status.short true
```

---

## Team rules reminder (important)

- No direct pushes to `main` once branch protection is enabled.
- Small PRs are easier to review and safer to merge.
- If in doubt, prefer `git revert` over rewriting history on shared branches.
