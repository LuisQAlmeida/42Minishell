# Testing and Validation

This directory defines the maintained testing and validation model for the
repository.

It describes what can be verified today, which checks are manual, what the
current CI workflow actually guarantees and where historical validation
evidence is preserved.

It must not present historical project testing as current automated coverage.

## Current State

The maintained repository currently has:

| Capability | Current state |
| --- | --- |
| Local build validation | Available through `make` |
| Pull-request build validation | Automated through GitHub Actions |
| Manual behavioural validation | Maintained practice |
| Memory validation | Manual, using Valgrind where relevant |
| File-descriptor validation | Manual, using Valgrind `--track-fds=yes` where relevant |
| Automated regression tests | Not implemented |
| Automated behavioural tests in CI | Not implemented |
| Static-analysis automation | Not implemented |
| Coverage reporting | Not implemented |

A successful CI run currently demonstrates that the project builds in the CI
environment. It does not demonstrate complete behavioural correctness,
resource safety or regression coverage.

## Maintained Documentation

The current validation model is documented in:

- [`validation-strategy.md`](validation-strategy.md), which defines validation
  layers, expectations by change type, evidence standards, CI boundaries and
  known testing gaps;
- [`manual-validation.md`](manual-validation.md), which provides reproducible
  operational checks for build, behaviour, signals, memory, file descriptors
  and repository changes.

## Historical Validation Evidence

Original 42 evaluation preparation and project-era validation remain preserved
under:

[`../history/validation/`](../history/validation/)

The historical material currently includes:

- [`mandatory-test-matrix.md`](../history/validation/mandatory-test-matrix.md),
  a broad mandatory-scope manual evaluation matrix;
- [`MSH-12-expansion-tests.md`](../history/validation/MSH-12-expansion-tests.md),
  focused expansion and exit-status validation evidence;
- [`MSH-14-builtin-tests.md`](../history/validation/MSH-14-builtin-tests.md),
  detailed builtin, build, Norminette, Valgrind and file-descriptor validation
  evidence.

These documents are useful evidence of how the original project was validated.

They are not an automated test suite and are not silently promoted to current
test results.

## Validation Evidence

Current validation claims should identify what was actually checked.

Examples include:

```text
make
manual command sequence
Valgrind invocation
file-descriptor inspection
documentation link audit
configuration-schema validation
GitHub functional verification
```

A pull request should not claim a category of validation merely because a
historical document contains tests for that category.

## Related Documentation

- [`../development/git-workflow.md`](../development/git-workflow.md) defines
  where validation fits into the issue-to-merge workflow.
- [`../architecture/resource-ownership.md`](../architecture/resource-ownership.md)
  documents memory and file-descriptor ownership.
- [`../architecture/process-and-signals.md`](../architecture/process-and-signals.md)
  documents process and signal behaviour relevant to validation.
- [`../history/README.md`](../history/README.md) defines the role of historical
  project material.
