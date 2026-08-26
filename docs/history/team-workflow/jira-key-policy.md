# Jira Key Policy (MSH) – Minishell

**Where to put this file:** `docs/project-management/jira-key-policy.md`

This policy standardizes how we reference Jira work items across **branches**, **commits**, and **pull requests**.

---

## 1) Project key

- Jira project key: **MSH**

All work tracked in Jira will use keys like:
- `MSH-1`, `MSH-2`, `MSH-42`, …

---

## 2) Before Jira exists (bootstrap phase)

Until the Jira project is created, we use a temporary umbrella key:

- **`MSH-SETUP`**

Use `MSH-SETUP` only for:
- repository scaffolding (folders, templates, CI)
- documentation about workflow/process
- BMad installation and planning artifacts
- anything that is clearly “setup”, not product functionality

### Examples (bootstrap)
- Branch: `chore/MSH-SETUP-add-ci-workflow`
- Commit: `MSH-SETUP chore: add CI workflow`
- PR title: `MSH-SETUP chore: add repo scaffolding`

---

## 3) After Jira exists (normal work)

Once the Jira project is created, every piece of work must map to a Jira issue key.

### Required rule
- **Every branch and PR must reference exactly one Jira key**
- **Every commit should reference a Jira key** (preferred)

### Examples (normal work)
- Branch: `feature/MSH-21-tokenize-operators`
- Commit: `MSH-21 feat: tokenize operators | < > << >>`
- PR title: `MSH-21 feat: tokenize operators`

---

## 4) Commit message format

Use this format:

`<JIRA_KEY> <type>: <summary>`

Where `<type>` is one of:
- `feat` (new functionality)
- `fix` (bug fix)
- `refactor` (internal restructuring, no behavior change)
- `docs` (documentation only)
- `chore` (tooling/config/maintenance)
- `test` (tests only)

### Examples
- `MSH-10 docs: add architecture overview`
- `MSH-42 fix: close unused pipe ends in parent`
- `MSH-18 feat: implement $VAR expansion`

---

## 5) One ticket vs multiple tickets

### One Jira issue can have multiple commits
That’s normal. Example:
- `MSH-21 feat: add operator tokens`
- `MSH-21 fix: handle << and >> precedence`
- `MSH-21 test: add lexer operator cases`

### Do not reuse the same key for unrelated work
If the work is conceptually different, create a new Jira ticket and use its key.

---

## 6) PR rules

- PR title should include the Jira key: `MSH-### <type>: ...`
- PR description should link the Jira ticket (or include the key in the “Jira” field)
- If a PR touches bonus work, it must still reference a Jira key and must be blocked until mandatory sign-off per our scope policy.

---

## 7) Bonus vs mandatory

Jira key policy does not change by scope:
- Mandatory and bonus both use `MSH-###`

Scope is tracked in Jira via:
- `Scope` field (Mandatory / Bonus) or labels (`mandatory`, `bonus`)

Bonus work should remain parked until mandatory is complete (see Working Agreements).
