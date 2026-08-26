# Definition of Done (DoD)

A story is **Done** when:

- [ ] Acceptance criteria are met
- [ ] `make` succeeds with required flags (`-Wall -Wextra -Werror`)
- [ ] Norm checks pass for touched files (if applicable)
- [ ] No leaks from our code in tested paths (Valgrind where relevant)
- [ ] No FD leaks in tested paths (pipes/redirections where relevant)
- [ ] Reviewed and approved by **both teammates**
- [ ] Merged into `main`
- [ ] Jira ticket updated with:
  - tests run / commands used
  - notes on edge cases or known limitations
