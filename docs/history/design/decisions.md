# Architecture Decisions (ADRs) – Minishell

This file is a lightweight Architecture Decision Record (ADR) log.
It captures *final* decisions (not brainstorming), so the team and evaluators can understand **what we chose and why**.

> Rule: research and exploration live in `docs/research/`.  
> Final decisions are summarized here.

---

## How to use this file

When a decision is made:
1. Add a new entry using the template below
2. Link to any supporting research note(s) in `docs/research/...`
3. Keep entries short and factual (easy to review)

---

## ADR Template

### ADR-YYYY-MM-DD: <Decision Title>
- **Status:** Accepted / Proposed / Rejected / Superseded
- **Scope:** Mandatory / Bonus
- **Subsystem:** Parser / Expansion / Builtins / Execution / Pipes / Redirections / Signals / Testing / Docs
- **Related Jira:** MSH-??? (or `MSH-SETUP` pre-Jira)
- **Context:** What problem are we solving? What constraints from the subject apply?
- **Decision:** What did we choose?
- **Rationale:** Why this option (trade-offs)?
- **Consequences:** What does this enable/limit? Any risks?
- **Mitigations:** How we reduce risks (tests, checks, constraints)?
- **References:** Links to research notes, bash experiments, or external references

---

## Decisions

### ADR-YYYY-MM-DD: (example) Token model vs AST
- **Status:** Proposed
- **Scope:** Mandatory
- **Subsystem:** Parser
- **Related Jira:** MSH-SETUP
- **Context:** Minishell mandatory requires parsing pipelines and redirections, but not complex operator precedence (bonus features parked).
- **Decision:** Use an AST-less command list model: tokens → command nodes (argv + redirs) → pipeline list.
- **Rationale:** Simpler to implement and explain; matches mandatory scope; reduces risk of overengineering.
- **Consequences:** Bonus operators may require extending parser later, but mandatory remains clean.
- **Mitigations:** Keep parser interfaces modular; isolate bonus parsing behind new entry points.
- **References:** `docs/research/parser/token-model-vs-ast.md`
