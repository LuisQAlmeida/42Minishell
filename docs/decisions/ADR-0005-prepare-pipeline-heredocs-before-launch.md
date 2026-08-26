# ADR-0005: Prepare pipeline heredocs before launching command children

- **Status:** Accepted
- **Recorded:** 2026-08-26
- **Decision scope:** Maintained implementation
- **Rationale provenance:** Reconstructed from the final implementation and preserved alternatives

## Context

A heredoc is an input redirection whose contents must be collected
interactively before the consuming command can read from it.

In the maintained implementation, heredoc collection itself requires temporary
runtime resources:

- a pipe;
- a dedicated heredoc child;
- heredoc-specific signal behaviour;
- a wait operation in the parent;
- a readable descriptor retained for later command execution.

A pipeline can contain heredocs in more than one command stage.

For example:

```text
command1 << FIRST | command2 << SECOND | command3
```

The shell therefore needs to decide when heredoc input is collected relative
to creation of the actual pipeline command processes.

Launching command children while later heredocs are still being collected
would mix two different foreground activities:

```text
pipeline execution
heredoc acquisition
```

The maintained implementation avoids that overlap.

## Decision

Prepare every heredoc belonging to the pipeline before launching any pipeline
command child.

The maintained execution order is conceptually:

```text
parsed pipeline
      |
      v
walk command chain
      |
      +--> prepare heredocs for command 1
      +--> prepare heredocs for command 2
      +--> prepare heredocs for command N
      |
      v
all heredoc preparation succeeded?
      |
   +--+--+
   |     |
  no    yes
   |     |
   v     v
abort   launch pipeline children
```

Each successfully prepared heredoc leaves a readable descriptor stored in its
corresponding `t_redir.fd`.

Only after the pipeline-wide heredoc pass succeeds does normal pipeline
construction begin.

The command child that eventually consumes the heredoc inherits the prepared
descriptor and applies it through the normal redirection path.

## Rationale

The final implementation clearly establishes a pipeline-wide heredoc
preparation phase.

The specific original rationale for choosing this order was not preserved
contemporaneously.

The rationale recorded here is therefore reconstructed from the final process,
signal and resource-ownership model.

Preparing all heredocs first creates a clean phase boundary:

```text
interactive heredoc acquisition
          |
          v
prepared redirection resources
          |
          v
pipeline process launch
```

This means pipeline command children do not begin normal command execution
while the parent is still collecting later heredoc input.

It also gives heredoc interruption a well-defined failure boundary.

If heredoc preparation is interrupted or otherwise fails, pipeline launch can
stop before command children are created.

The resulting readable descriptors are prepared resources rather than active
pipeline topology.

They can remain attached to their `t_redir` nodes until the relevant command
child applies its redirections.

This fits the maintained redirection ownership model:

```text
heredoc acquisition
      |
      v
pipe read end
      |
      v
t_redir.fd
      |
      | inherited through later fork
      v
command child
      |
      v
redirection application
      |
      v
STDIN
```

These properties are consequences of the final architecture and provide a
reasonable explanation for the ordering.

They are not presented as confirmed historical motivations for the original
implementation choice.

## Alternatives Considered

### Prepare each heredoc immediately before its command child

A pipeline could be constructed incrementally so that each stage's heredocs
are collected immediately before that stage is forked.

Conceptually:

```text
prepare stage 1 heredocs
fork stage 1

prepare stage 2 heredocs
fork stage 2

prepare stage 3 heredocs
fork stage 3
```

This could reduce the time between heredoc preparation and consumption.

However, earlier pipeline children could already be running while the shell is
still performing interactive heredoc acquisition for later stages.

That would couple foreground pipeline execution with readline-driven heredoc
collection.

It would also make interruption during a later heredoc occur after part of the
pipeline had already been launched.

The maintained implementation does not use this ordering.

This alternative is an analytical comparison with the final architecture, not
a preserved claim that this exact launch strategy was formally debated.

### Collect heredocs inside command children

Another possible design would defer heredoc acquisition to the child that
eventually consumes the input.

That would colocate acquisition and consumption in the same process.

However, heredoc reading requires interactive terminal handling and a
specialized signal policy before normal command execution.

Performing that interaction independently inside pipeline command children
would complicate foreground terminal ownership and coordination between
multiple stages.

The maintained implementation instead gives heredoc acquisition its own
temporary child process and completes that phase before pipeline command
children are launched.

This alternative is inferred from the process architecture rather than
documented as an original design debate.

## Consequences

### Positive

- all interactive heredoc acquisition finishes before pipeline execution
  begins;
- interruption during heredoc preparation can abort the pipeline before command
  children are launched;
- pipeline command children receive already prepared heredoc descriptors;
- heredoc resources integrate with the normal `t_redir.fd` ownership model;
- heredoc-specific signal handling remains separate from normal execution-child
  signal behaviour;
- pipeline launch operates on a fully prepared set of heredoc inputs;
- command execution does not need to invoke readline for heredoc acquisition.

### Trade-offs

- every pipeline heredoc must be collected before any command stage begins
  executing;
- prepared heredoc descriptors can remain open across subsequent heredoc
  collection until pipeline launch;
- the execution layer requires a distinct pipeline-wide heredoc pre-pass;
- heredoc preparation and normal file-redirection preparation do not follow
  exactly the same pipeline timing;
- the model introduces additional process and descriptor lifecycle steps before
  the first pipeline command child is launched.

## Evidence

### Final implementation

The maintained source verifies the decision through:

- `src/exec/pipeline_wait.c`
  - pipeline heredoc preparation traverses command stages before normal
    pipeline launch proceeds;
  - `exe_heredocs_prepare()` is invoked for commands during that pre-pass;
- `src/exec/redir_prepare.c`
  - `exe_heredocs_prepare()` prepares heredoc redirections;
  - successful preparation stores the resulting readable descriptor in the
    corresponding redirection;
- `src/exec/heredoc.c`
  - creates the heredoc pipe;
  - forks the dedicated heredoc child;
  - waits for heredoc completion in the parent;
  - retains the pipe read end when preparation succeeds;
- `src/exec/heredoc_read.c`
  - performs heredoc input collection;
- `src/exec/heredoc_terminal.c`
  - saves and restores terminal state around heredoc acquisition;
- `src/exec/pipeline.c`
  - launches pipeline command children only after the preceding pipeline
    preparation path succeeds;
- `include/minishell.h`
  - exposes the heredoc preparation and pipeline execution interfaces.

The final runtime therefore separates pipeline-wide heredoc acquisition from
pipeline command-process creation.

### Preserved development material

`docs/history/design/minishell-architecture.md` records that heredoc input
should be prepared before the command that consumes it executes and that the
result should be represented by a readable descriptor suitable for standard
input.

The preserved material does not explicitly describe the final policy of
preparing all heredocs across the entire pipeline before launching any command
child.

It therefore supports the general preparation-before-execution requirement but
does not establish the specific rationale for the pipeline-wide pre-pass.

For that reason, the rationale in this ADR is explicitly retrospective.

## References

Maintained architecture:

- [`../architecture/runtime-flow.md`](../architecture/runtime-flow.md)
- [`../architecture/process-and-signals.md`](../architecture/process-and-signals.md)
- [`../architecture/resource-ownership.md`](../architecture/resource-ownership.md)

Preserved development material:

- [`../history/design/minishell-architecture.md`](../history/design/minishell-architecture.md)
