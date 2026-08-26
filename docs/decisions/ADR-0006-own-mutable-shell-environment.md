# ADR-0006: Maintain an owned mutable environment in shell state

- **Status:** Accepted
- **Recorded:** 2026-08-26
- **Decision scope:** Maintained implementation
- **Rationale provenance:** Reconstructed from the final implementation and preserved alternatives

## Context

Minishell needs environment state that persists across interactive command
iterations.

That state is used by several parts of the system:

- variable expansion reads environment values;
- `export` adds or updates entries;
- `unset` removes entries;
- `cd` updates `PWD` and `OLDPWD`;
- external command execution passes an environment to `execve()`;
- child processes inherit the current shell state at `fork()` time.

The environment received by `main()` is process input rather than an
application-owned mutable model.

The shell therefore needs to establish what representation owns the persistent
environment and how mutations are reflected across later commands.

Historical design material considered an explicit environment abstraction such
as a linked `t_env` structure, with environment arrays potentially created when
needed for `execve()`.

The maintained implementation uses a different representation.

## Decision

Store the persistent shell environment directly as an owned mutable `char **`
inside `t_shell`.

The maintained session state is conceptually:

```text
t_shell
├── envp
├── last_status
└── should_exit
```

At session initialization:

```text
process envp
      |
      | sta_env_copy()
      v
shell->envp
```

`sta_env_copy()` creates an independent environment array and duplicates its
entries.

From that point onward, `shell->envp` is the maintained environment source of
truth.

Environment operations use that representation directly:

```text
                    shell->envp
                         |
          +--------------+--------------+
          |              |              |
          v              v              v
    sta_env_value() sta_env_set() sta_env_unset()
          |              |              |
          v              v              v
      expansion      export / cd       unset
```

External command execution also uses the current `shell->envp` when invoking
`execve()`.

## Rationale

The final implementation clearly establishes an owned mutable `char **`
environment in `t_shell`.

The specific original rationale for choosing this representation over a
dedicated environment model was not preserved clearly enough to present it as
historical fact.

The rationale recorded here is therefore reconstructed from the final
architecture and preserved alternatives.

Using one persistent environment representation creates a direct relationship
between shell mutation and later consumption:

```text
builtin mutation
      |
      v
shell->envp
      |
      +--> later expansion
      |
      +--> later builtin
      |
      `--> later execve()
```

There is no separate persistent linked environment model that must be converted
back into an `envp` array before external command execution.

The representation also matches the interface required by `execve()`.

That means the environment owned by the interactive shell can be passed
directly to external programs after the appropriate child process is created.

The state subsystem still encapsulates lookup, insertion and removal logic, so
the use of `char **` as storage does not require callers to manipulate the
array structure themselves.

These properties are consequences of the final implementation and provide a
reasonable architectural rationale for the representation.

They are not presented as confirmed historical motivations for the original
choice.

## Ownership Model

The initial environment is deep-copied.

Conceptually:

```text
original process environment
          |
          | duplicate array
          | duplicate entries
          v
     shell-owned envp
```

The persistent shell therefore owns the strings and pointer array associated
with its environment copy.

Lookup does not transfer ownership.

`sta_env_value()` returns a pointer into the existing environment entry value.

Callers must treat that result as borrowed state.

When an owned expansion result is needed, the expansion layer duplicates the
value separately.

### Updating an existing entry

When an environment entry is replaced:

```text
existing entry
      |
      v
free old string
      |
      v
store new owned string
```

The persistent environment array remains the shell's representation.

### Adding an entry

When a new variable requires a larger array:

```text
old char ** array
      |
      | transfer existing string pointers
      v
new char ** array
      |
      +--> existing owned strings
      +--> new owned entry
      `--> NULL
```

The old outer pointer array is released after its entries have been
transferred.

### Removing an entry

When a variable is removed:

```text
old environment
      |
      +--> removed entry -> free
      |
      `--> retained entries
                |
                | transfer
                v
         new char ** array
```

The environment therefore remains owned by the shell even when the outer array
changes during mutation.

## Process Boundary

At `fork()`, an execution child inherits a process-local copy of the shell
state.

Conceptually:

```text
parent shell->envp
        |
        | fork()
        v
child-local environment state
```

A child can pass its inherited `shell->envp` to `execve()`.

Changes made to that environment inside a child process do not mutate the
persistent parent's copy.

This aligns with the separate execution-context decision documented for
builtins:

- standalone builtins can mutate the parent environment;
- pipeline builtins mutate only child-local state.

The persistent source of truth therefore remains with the interactive parent.

## Alternatives Considered

### Dedicated `t_env` representation

Preserved architecture material considered a dedicated environment-storage
type, potentially represented as linked nodes.

Conceptually:

```text
t_env
  |
  +--> name
  +--> value
  +--> export metadata
  `--> next
```

Such a model could provide richer typed access to environment state and make
individual variable operations independent from array reconstruction.

It could also support metadata that does not map directly to `NAME=value`
strings.

However, external execution ultimately requires an environment array compatible
with `execve()`.

A separate persistent `t_env` representation would therefore require an
additional conversion or synchronization boundary.

The maintained implementation does not use that architecture.

### Borrow the original `envp`

Another possible implementation would retain the `envp` received by `main()`
without creating an owned copy.

That would avoid the initial duplication step.

However, Minishell performs persistent additions, replacements and removals.

An owned representation gives the state subsystem explicit control over the
lifetime of both the array and its entries.

The maintained implementation therefore establishes ownership during session
initialization.

This alternative is an analytical comparison with the final architecture, not
a preserved claim that borrowing the original array was formally debated.

### Maintain separate internal and execution representations

The shell could keep one internal environment model and generate a temporary
`char **envp` only when launching external commands.

This separates internal state design from the `execve()` interface.

It also introduces a synchronization and allocation boundary each time the
execution representation must be rebuilt.

The final implementation instead keeps a representation that already matches
the external execution interface.

Historical design material considered temporary `envp` arrays alongside a
possible internal environment store, but the maintained source does not retain
that split.

## Consequences

### Positive

- the shell has one persistent environment source of truth;
- environment mutations survive across prompt iterations;
- lookup, insertion and removal operate on the same maintained representation;
- external commands can receive the current environment directly through
  `execve()`;
- no persistent `t_env` to `char **` synchronization layer is required;
- ownership of environment memory is explicit at session level;
- child processes naturally inherit a process-local snapshot through `fork()`.

### Trade-offs

- adding and removing variables can require allocation of a new pointer array;
- entries are represented as `NAME=value` strings rather than richer typed
  objects;
- variable lookup requires searching the environment array;
- metadata not encoded in environment strings would require an additional
  representation;
- ownership transfers during array replacement and removal require careful
  memory management;
- the persistent environment model is coupled to the conventional `envp`
  representation expected by process execution.

## Evidence

### Final implementation

The maintained source verifies the decision through:

- `include/minishell.h`
  - `t_shell` owns a `char **envp` field;
  - state operations accept or update the shell environment;
- `src/session/prompt_loop.c`
  - `ses_init()` initializes `shell->envp` with `sta_env_copy()`;
- `src/state/env_copy.c`
  - `sta_env_copy()` creates the initial owned environment copy;
- `src/state/env_lookup.c`
  - environment lookup and indexing operate directly on `char **envp`;
- `src/state/env_set.c`
  - `sta_env_set()` replaces existing entries or extends the environment
    array;
- `src/state/env_unset.c`
  - `sta_env_unset()` removes entries and replaces the outer environment
    array;
- the execution layer passes the maintained shell environment to external
  command execution.

The final source contains no persistent `t_env` abstraction.

### Preserved development material

`docs/history/design/minishell-architecture.md` considered the environment
source of truth as either an environment list or environment array.

It also described a possible `t_env` linked representation and temporary
`envp` arrays for external execution.

The final implementation resolves those alternatives by storing the persistent
environment directly as an owned `char **` in `t_shell`.

The preserved material does not record the specific original rationale for that
final representation.

For that reason, the rationale in this ADR is explicitly retrospective.

## References

Maintained architecture:

- [`../architecture/system-overview.md`](../architecture/system-overview.md)
- [`../architecture/runtime-flow.md`](../architecture/runtime-flow.md)
- [`../architecture/process-and-signals.md`](../architecture/process-and-signals.md)
- [`../architecture/resource-ownership.md`](../architecture/resource-ownership.md)

Related decision:

- [`ADR-0003`](ADR-0003-run-standalone-builtins-in-parent.md)

Preserved development material:

- [`../history/design/minishell-architecture.md`](../history/design/minishell-architecture.md)
