# ADR-0001: Perform variable expansion while quote context is available

- **Status:** Accepted
- **Recorded:** 2026-08-26
- **Decision scope:** Maintained implementation
- **Rationale provenance:** Supported by preserved development material and the final source implementation

## Context

Minishell must interpret environment-variable references and the previous exit
status differently depending on quoting context.

The maintained implementation distinguishes three relevant word-segment
contexts:

- unquoted text;
- single-quoted text;
- double-quoted text.

Their expansion behaviour is different:

| Context | Variable expansion |
| --- | --- |
| Unquoted text | Yes |
| Single-quoted text | No |
| Double-quoted text | Yes |

The scanner also removes quote delimiters while constructing the final word.

Once those delimiters have been discarded, a later processing stage cannot
reliably determine whether characters in the resulting word originally came
from unquoted, single-quoted or double-quoted input.

The expansion decision therefore depends on lexical context that exists during
scanning but is not preserved in the resulting token model.

## Decision

Perform mandatory variable expansion while word segments are being scanned and
their quote context is still known.

The maintained flow is conceptually:

```text
input line
    |
    v
scan word
    |
    +--> unquoted segment
    |       |
    |       `--> expand variables
    |
    +--> single-quoted segment
    |       |
    |       `--> preserve literally
    |
    `--> double-quoted segment
            |
            `--> expand variables
    |
    v
assembled TOK_WORD
```

Expansion is therefore part of word construction rather than a separate pass
over parsed commands.

The resulting token contains the already processed word value needed by the
grammar and execution layers.

## Rationale

The decision keeps expansion at the point where the implementation still knows
how each source segment was quoted.

This allows the scanner to apply the required distinction directly:

```text
unquoted       -> expansion
single quoted  -> literal content
double quoted  -> expansion
```

The preserved `MSH-12` expansion documentation explicitly records this
rationale: expansion occurs during tokenization because quote characters are
removed by that stage, and postponing expansion would lose the information
needed to distinguish the original quote context.

The final source retains the same architectural property.

`scn_word()` dispatches segment handling according to lexical context.
Unquoted segments are expanded through `exp_variables()`.

`scn_quote_single()` returns literal quoted content without variable
expansion.

`scn_quote_double()` processes the double-quoted segment through
`exp_variables()` before returning it to word construction.

The grammar therefore receives word tokens whose mandatory variable expansion
has already been resolved.

## Alternatives Considered

### Separate expansion after parsing

Earlier preserved architecture material described the conceptual lifecycle as:

```text
readline -> tokenize -> parse -> expand -> execute
```

Under that model, expansion would operate on parsed command words and
redirection operands after lexical processing.

This separation has an attractive subsystem boundary: tokenization performs
lexical analysis, parsing constructs command structures, and expansion then
transforms those structures before execution.

However, the final scanner does not retain quote-origin metadata after it
removes the quote delimiters.

A separate later expansion stage would therefore require additional
representation state, such as quote metadata attached to tokens or parsed word
segments, in order to preserve the same semantics correctly.

The maintained implementation instead resolves expansion before that
information disappears.

### Preserve quote metadata for later expansion

Another possible architecture would retain enough quote information in the
token or command representation for a later expansion phase.

That could preserve a formally separate expansion subsystem.

The maintained implementation does not use such a representation.

Introducing it would increase the amount of transient parsing state and change
the current token-to-command data model without being required for the
mandatory implementation.

This is an architectural alternative inferred from the representation
constraints, not a preserved claim that it was formally debated during the
original project.

## Consequences

### Positive

- quote-sensitive expansion is resolved while the required lexical context is
  directly available;
- single-quoted content can remain literal without reconstructing quote
  history later;
- grammar operates on already expanded `TOK_WORD` values;
- execution does not need a separate mandatory variable-expansion phase;
- the token and command models do not need to preserve general quote metadata;
- the implementation boundary is straightforward to trace from source input to
  final word value.

### Trade-offs

- scanning and expansion are architecturally coupled;
- lexical processing is responsible for more than token classification alone;
- adding expansion features that require parsed command context may not fit
  naturally into the same stage;
- the implementation differs from a conventional architecture where lexing,
  parsing and expansion are fully separate passes;
- future features that require quote metadata after scanning would need either
  additional representation state or a revised expansion boundary.

## Evidence

### Final implementation

The maintained source verifies the decision through:

- `src/scan/word_scan.c`
  - `scn_word()`;
  - unquoted segment processing through `exp_variables()`;
- `src/scan/quote_scan.c`
  - `scn_quote_single()`;
  - `scn_quote_double()`;
- `src/expand/variables.c`
  - `exp_variables()`;
- `include/minishell.h`
  - scanner and expansion interfaces.

The final scanner expands unquoted and double-quoted segments while preserving
single-quoted segments literally.

No separate post-grammar mandatory variable-expansion pass exists in the
maintained runtime.

### Preserved development material

`docs/history/design/MSH-12-expansion-flow.md` explicitly records that
expansion happens during tokenization while quote context is still available.

It also explains that tokenization removes quote characters, making that timing
important for preserving the distinction between unquoted, single-quoted and
double-quoted input.

Earlier `docs/history/design/minishell-architecture.md` described expansion as
a separate stage after parsing.

Together, these documents preserve both the earlier architectural model and
the rationale for the implemented flow that replaced it.

## References

Maintained architecture:

- [`../architecture/system-overview.md`](../architecture/system-overview.md)
- [`../architecture/runtime-flow.md`](../architecture/runtime-flow.md)

Preserved development material:

- [`../history/design/MSH-12-expansion-flow.md`](../history/design/MSH-12-expansion-flow.md)
- [`../history/design/minishell-architecture.md`](../history/design/minishell-architecture.md)
