# ADR-0002: Represent pipelines as linked command chains

- **Status:** Accepted
- **Recorded:** 2026-08-26
- **Decision scope:** Maintained implementation
- **Rationale provenance:** Supported by preserved development material and the final source implementation

## Context

The mandatory Minishell grammar needs to represent commands connected by
pipelines and commands containing ordered redirections.

It does not require the maintained mandatory implementation to model a general
shell expression language with operator precedence, grouped expressions or
nested boolean operators.

The relevant command structure is therefore fundamentally linear:

```text
command
   |
   v
command
   |
   v
command
```

Each command must retain its own:

- argument vector;
- argument count;
- ordered redirection list;
- link to the next pipeline command.

A general abstract syntax tree could represent this grammar, but it would add a
more expressive structural model than the maintained mandatory execution path
requires.

## Decision

Represent a pipeline directly as a linked chain of `t_cmd` nodes.

The maintained command type is conceptually:

```text
t_cmd
├── argv
├── argc
├── redirs
└── next
      |
      v
    t_cmd
      |
      v
    t_cmd
```

`grm_pipeline()` constructs this chain by splitting the token stream at pipe
tokens and creating one command node for each pipeline stage.

There is no separate maintained `t_pipeline` container and no general AST.

Execution receives the first `t_cmd` node and traverses the linked command
chain.

## Rationale

The preserved design material explicitly preferred a linear command-list model
over a full AST for the mandatory project scope.

The mandatory grammar does not require precedence relationships between several
classes of expression operators.

Instead, the core structural relationship is:

```text
pipeline
    |
    +--> command
    +--> command
    `--> command
```

with redirections attached to the individual command they affect.

A linked command chain represents that relationship directly.

This keeps the grammar-to-execution handoff small:

```text
tokens
   |
   v
grm_pipeline()
   |
   v
linked t_cmd chain
   |
   v
simple or pipeline execution
```

The final implementation simplified the preserved architecture further.

Historical design material described the possibility of a `t_pipeline`
structure containing command nodes.

The maintained source does not retain that wrapper. The `t_cmd` links
themselves provide the pipeline sequence needed by execution.

The architectural decision that survives is therefore the use of a linear,
AST-less command representation rather than the exact historical container
shape.

## Alternatives Considered

### Full abstract syntax tree

The preserved architecture material explicitly considered the distinction
between an AST and a simpler command-list representation.

A full AST would become useful when the grammar must encode relationships such
as:

```text
AND / OR precedence
parenthesized groups
nested expressions
pipeline expressions used as tree leaves
```

That representation would allow execution to recursively evaluate a richer
shell grammar.

For the maintained mandatory scope, those relationships are not represented.

Using a full AST would therefore introduce node types, recursive traversal and
additional ownership rules that are not required by the implemented grammar.

The preserved design material intentionally kept those concerns outside the
mandatory parser.

### Separate `t_pipeline` container

Earlier architecture material described a possible `t_pipeline` structure that
would contain or reference the command sequence.

This preserves a distinct type for the pipeline abstraction and can provide a
place for pipeline-wide metadata.

The final implementation does not require such a container.

Pipeline-wide execution state that is needed at runtime is represented
separately by execution-specific structures such as `t_pipe_state`, while the
parsed pipeline itself remains a linked `t_cmd` chain.

Removing the wrapper reduces one level of representation without changing the
linear execution semantics.

## Consequences

### Positive

- the parsed representation closely matches the mandatory grammar;
- pipeline order is explicit through `t_cmd->next`;
- each command owns its own arguments and redirections;
- grammar construction does not require general AST node machinery;
- execution can traverse pipeline stages directly;
- simple commands and pipeline stages use the same command representation;
- ownership and cleanup remain straightforward through one linked command
  chain.

### Trade-offs

- the representation is specialized for linear pipelines;
- richer expression grammar cannot be represented naturally by `t_cmd->next`
  alone;
- adding boolean operators, grouped expressions or precedence would require a
  higher-level representation;
- pipeline-wide parsed metadata has no dedicated container and must either be
  derived from the chain or represented separately at execution time;
- the final representation differs from historical diagrams that included a
  distinct `t_pipeline` type.

## Evidence

### Final implementation

The maintained source verifies the decision through:

- `include/minishell.h`
  - `t_cmd` contains `struct s_cmd *next`;
  - no maintained `t_pipeline` type exists;
- `src/grammar/command_build.c`
  - creates individual `t_cmd` nodes;
- `src/grammar/command_chain.c`
  - appends command nodes;
  - `grm_pipeline()` builds the complete linked command chain;
  - `grm_count()` counts commands by traversing that chain;
- `src/grammar/command_memory.c`
  - `grm_clear()` destroys the linked command representation;
- `src/exec/pipeline.c`
  - pipeline execution traverses the resulting command chain.

No general AST is constructed in the maintained mandatory runtime.

### Preserved development material

`docs/history/design/decisions.md` records an AST-less command-list model for
mandatory parsing.

`docs/history/design/minishell-architecture.md` states that the mandatory
implementation should prefer a linear pipeline model over a complex AST and
explains that the mandatory syntax does not require general operator
precedence.

That material also describes a possible `t_pipeline` representation.

The final implementation preserves the linear, AST-less architectural choice
while simplifying the concrete representation to a direct linked `t_cmd`
chain.

## References

Maintained architecture:

- [`../architecture/system-overview.md`](../architecture/system-overview.md)
- [`../architecture/runtime-flow.md`](../architecture/runtime-flow.md)

Preserved development material:

- [`../history/design/decisions.md`](../history/design/decisions.md)
- [`../history/design/minishell-architecture.md`](../history/design/minishell-architecture.md)
