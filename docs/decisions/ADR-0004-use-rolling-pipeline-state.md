# ADR-0004: Manage pipelines with rolling descriptor state

- **Status:** Accepted
- **Recorded:** 2026-08-26
- **Decision scope:** Maintained implementation
- **Rationale provenance:** Reconstructed from the final implementation and preserved alternatives

## Context

Executing a pipeline requires each command stage to receive the appropriate
input and output endpoints while unused pipe descriptors are closed promptly.

For a pipeline such as:

```text
producer | transform | consumer
```

the shell must coordinate:

- the read endpoint inherited from the previous stage;
- the pipe created for the next stage;
- the PID of the final stage whose status determines the pipeline result;
- closure of descriptors that the parent and children no longer need.

One possible implementation strategy is to allocate and retain all `N - 1`
pipes for an `N`-command pipeline before launching the command processes.

The maintained implementation instead traverses the linked command chain from
left to right and keeps only the pipeline descriptor state required for the
current transition.

## Decision

Manage pipeline construction using a rolling `t_pipe_state`.

The maintained state is conceptually:

```text
t_pipe_state
├── prev_fd
├── pipefd[2]
└── last_pid
```

where:

```text
prev_fd
    read endpoint produced by the previous pipeline stage

pipefd[0]
    read endpoint of the pipe created for the current stage transition

pipefd[1]
    write endpoint of the pipe created for the current stage transition

last_pid
    PID of the most recently launched stage, which becomes the final-stage PID
    after pipeline construction completes
```

Pipeline construction advances one command at a time.

For each command:

```text
current command
      |
      +--> create next pipe if another command follows
      |
      +--> fork current stage
      |
      +--> remember child PID
      |
      +--> close previous parent read endpoint
      |
      +--> close current parent write endpoint
      |
      `--> retain current read endpoint as next prev_fd
```

The parent therefore carries only the descriptor state needed to connect the
next command.

## Rationale

The final implementation clearly establishes rolling pipeline state, but the
specific original rationale for choosing this form was not preserved
contemporaneously.

The rationale recorded here is therefore reconstructed from the resulting
architecture and from the alternative pipeline model preserved in earlier
design material.

The rolling representation matches the final parsed command model closely.

Commands are already represented as a left-to-right linked `t_cmd` chain, so
pipeline construction can consume that chain incrementally:

```text
t_cmd
  |
  v
launch
  |
  v
advance state
  |
  v
next t_cmd
```

At each step, the parent only needs:

```text
previous read end
current pipe pair
current last PID
```

Once a child has inherited the descriptors required for its stage, the parent
can close descriptors that no longer participate in later pipeline
construction.

This keeps descriptor lifetime explicit and local to the command transition
that uses it.

It also avoids requiring a separate pipeline-wide array of all pipe pairs in
the maintained implementation.

These are architectural consequences and reasonable motivations inferred from
the final design.

They are not presented as confirmed historical reasons for the original
implementation choice.

## Alternatives Considered

### Pre-create all `N - 1` pipes

Preserved architecture material described the pipeline conceptually as:

```text
for N commands:
    create N - 1 pipes
    connect each child to the required pipe endpoints
    close unused descriptors
    wait for all children
```

This is a common and valid pipeline representation.

It makes the complete descriptor topology available before any command child
is launched.

It can also make stage-to-pipe indexing explicit because all pipe pairs exist
in one pipeline-wide structure.

The maintained implementation does not use this model.

Instead, it creates the next pipe only when another command follows and carries
the resulting read endpoint forward through `prev_fd`.

### Store all pipeline descriptors in a dynamic structure

Another possible design would allocate an array or other container for every
pipe descriptor in the pipeline.

That could centralize pipeline-wide cleanup and make arbitrary descriptor
lookup possible.

The final implementation does not require arbitrary access to earlier pipe
pairs once a stage has been launched.

A rolling state therefore represents the actual traversal needs with less
persistent execution metadata.

This alternative is an analytical comparison with the final architecture, not
a preserved claim that a dynamic descriptor container was formally debated
during the original project.

## Consequences

### Positive

- pipeline construction matches the left-to-right linked command traversal;
- the parent tracks only the previous read endpoint and current pipe pair;
- descriptors can be closed as soon as they are no longer required by future
  stages;
- execution does not require a pipeline-wide pipe array;
- descriptor ownership remains localized to the current transition;
- the final stage PID is tracked naturally while commands are launched;
- the implementation scales with pipeline length without accumulating all pipe
  descriptors in one parent-side representation.

### Trade-offs

- the complete pipeline descriptor topology is not available in one structure;
- execution logic depends on correctly advancing `prev_fd` after every stage;
- failure cleanup must understand partially advanced rolling state;
- debugging a complete pipeline can require following descriptor transitions
  over time rather than inspecting one pre-built pipe table;
- the model is specialized for sequential left-to-right pipeline construction.

## Evidence

### Final implementation

The maintained source verifies the decision through:

- `include/minishell.h`
  - `t_pipe_state` contains `prev_fd`, `pipefd[2]` and `last_pid`;
- `src/exec/pipeline.c`
  - initializes rolling pipeline state;
  - creates a new pipe only when another command follows;
  - forks one stage at a time;
  - records each launched PID in `last_pid`;
  - closes the previous read endpoint in the parent;
  - closes the current write endpoint in the parent;
  - moves the current read endpoint into `prev_fd`;
- `src/exec/pipeline_child.c`
  - duplicates `prev_fd` into standard input when required;
  - duplicates the current write endpoint into standard output when required;
  - closes inherited pipeline descriptors after duplication;
- `src/exec/pipeline_wait.c`
  - waits for pipeline children;
  - returns the status associated with `last_pid`.

The maintained source does not allocate an array containing all `N - 1` pipe
pairs.

### Preserved development material

`docs/history/design/minishell-architecture.md` describes pipeline execution
conceptually using `N - 1` pipes for `N` commands.

It also records two principles that remain visible in the final
implementation:

- children should close unused pipe descriptors promptly;
- the parent should close pipe ends as soon as they are no longer needed.

The preserved document does not record the later transition to the final
rolling `t_pipe_state` representation or the specific motivation for that
change.

For that reason, the rationale in this ADR is explicitly retrospective.

## References

Maintained architecture:

- [`../architecture/runtime-flow.md`](../architecture/runtime-flow.md)
- [`../architecture/process-and-signals.md`](../architecture/process-and-signals.md)
- [`../architecture/resource-ownership.md`](../architecture/resource-ownership.md)

Preserved development material:

- [`../history/design/minishell-architecture.md`](../history/design/minishell-architecture.md)
