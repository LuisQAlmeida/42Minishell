## Jira
- Ticket: MSH-???

## What changed
- 

## Why
- 

## How to test
- [ ] Build: `make`
- [ ] Run manual tests:
  - 
- [ ] Valgrind / leaks (if relevant):
  - 

## Scope & Subsystem
- Scope: Mandatory / Bonus
- Subsystem: Parser / Expansion / Builtins / Execution / Pipes / Redirections / Signals / Testing / Docs

## Checklist
- [ ] Subject scope respected (no accidental bonus/extra features)
- [ ] Norminette checked on touched files (if applicable)
- [ ] No new leaks in tested paths (Valgrind where applicable)
- [ ] No FD leaks in tested paths (pipes/redirs)
- [ ] Exit status behavior (`$?`) verified where relevant
- [ ] Signal behavior verified where relevant
- [ ] Both teammates understand the change
