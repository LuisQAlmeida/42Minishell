# ADR-0003: Execute standalone builtins in the parent shell

- **Status:** Accepted
- **Recorded:** 2026-08-26
- **Decision scope:** Maintained implementation
- **Rationale provenance:** Supported by preserved development material and the final source implementation

## Context

Some shell builtins modify state that must survive after the command completes.

Examples in the maintained implementation include:

- `cd`, which changes the shell's working directory and updates `PWD` and
  `OLDPWD`;
- `export`, which modifies the shell environment;
- `unset`, which removes variables from the shell environment;
- `exit`, which updates the persistent shell exit state.

If such a builtin executes only inside a forked child process, its memory and
process-state changes disappear when that child terminates.

The shell therefore needs an execution context that distinguishes standalone
builtins from commands executed as pipeline stages.

Parent execution introduces another requirement.

A standalone builtin may contain redirections:

```text
export NAME=value > output
cd /tmp > log
```

Applying those redirections directly to the persistent shell without restoring
its standard descriptors would affect later commands and the interactive
prompt.

## Decision

Execute standalone builtins directly in the persistent parent shell.

The maintained simple-command dispatch is conceptually:

```text
simple command
      |
      v
prepare redirections
      |
      +--> no argv
      |       `--> redirection-only parent path
      |
      +--> builtin
      |       `--> execute in parent
      |
      `--> external command
              `--> fork execution child
```

All standalone builtins use the parent execution wrapper.

When parent execution requires redirections, preserve the shell's standard
streams through a save/apply/restore sequence:

```text
parent STDIN / STDOUT
        |
        v
      dup()
        |
        v
 saved descriptors
        |
        v
apply command redirections
        |
        v
execute builtin
        |
        v
restore with dup2()
        |
        v
close saved descriptors
```

When a builtin is part of a pipeline, execute it in the pipeline child instead.

Its state changes are therefore local to that child and do not mutate the
persistent parent shell.

## Rationale

Persistent shell state belongs to the interactive parent process.

Executing state-changing standalone builtins there allows their effects to
survive into subsequent prompt iterations.

The relationship is direct:

```text
standalone cd
      |
      v
parent process
      |
      +--> chdir()
      +--> update PWD / OLDPWD
      |
      v
next prompt observes new state
```

The same principle applies to environment mutation and session exit state.

The preserved architecture material explicitly identifies `cd`, `export`,
`unset` and `exit` as builtins that require parent execution when used
standalone.

It also records that parent-run builtins may require temporary redirections and
that the original standard descriptors must be restored afterward.

The final implementation generalizes the standalone path slightly further:
all recognized standalone builtins execute through `blt_execute_parent()`,
including builtins such as `echo`, `pwd` and `env` that do not normally require
persistent state mutation.

This gives standalone builtin dispatch one consistent execution path.

Pipeline semantics create the opposite boundary.

A builtin in a pipeline must behave as one process stage in concurrent
process-style data flow.

Executing it in the pipeline child naturally gives it the required pipe
endpoints while preventing state mutations from leaking back into the
interactive parent.

## Alternatives Considered

### Execute every builtin in a child

A uniform child-only model would simplify process dispatch because builtins and
external commands could always use the same forked execution context.

It would also isolate command redirections naturally inside the child.

However, changes made by stateful builtins would disappear when the child exits.

For example:

```text
cd /tmp
```

would not change the working directory of the interactive shell.

Likewise, standalone `export`, `unset` and `exit` would fail to persist their
intended shell-state effects.

This alternative is incompatible with the required behaviour of stateful
standalone builtins.

### Execute pipeline builtins in the parent

Another possible model would attempt to preserve builtin state by executing
pipeline builtins in the interactive parent.

That would complicate pipeline execution because a pipeline stage must
participate in concurrent descriptor-based data flow with neighboring stages.

It would also allow state-changing builtins inside pipelines to mutate the
persistent shell unexpectedly.

The preserved architecture material explicitly chose child execution for
builtins used as pipeline stages.

### Apply parent redirections without restoration

Parent execution could apply command redirections directly and leave the
modified descriptors in place.

That would make the implementation smaller locally, but the redirections would
survive beyond the builtin invocation.

Later prompts and commands could inherit redirected `stdin` or `stdout`.

The maintained implementation therefore treats parent redirections as a
temporary execution scope rather than persistent shell state.

## Consequences

### Positive

- standalone stateful builtins can modify persistent shell state;
- `cd`, `export`, `unset` and `exit` have effects visible after the command
  finishes;
- all standalone builtins share one consistent parent dispatch path;
- pipeline builtins participate naturally as child process stages;
- state mutations made inside pipeline builtins remain process-local;
- temporary parent redirections do not leak into later commands;
- builtin execution preserves the same command redirection semantics available
  to other command forms.

### Trade-offs

- builtin execution depends on command context rather than builtin identity
  alone;
- the persistent parent must temporarily manipulate its own standard file
  descriptors;
- save/apply/restore introduces failure paths that child-only execution would
  avoid;
- standalone builtin execution and pipeline builtin execution follow different
  process lifetimes;
- developers must reason carefully about whether a builtin invocation is
  expected to mutate persistent or child-local shell state.

## Evidence

### Final implementation

The maintained source verifies the decision through:

- `src/exec/command_simple.c`
  - standalone builtin detection dispatches to `blt_execute_parent()`;
  - external simple commands use a forked child path;
- `src/builtins/parent.c`
  - `blt_execute_parent()` saves parent standard descriptors;
  - applies command redirections;
  - executes the builtin;
  - restores the saved descriptors;
- `src/exec/stdio_backup.c`
  - `exe_stdio_save()`;
  - `exe_stdio_restore()`;
- `src/exec/pipeline_child.c`
  - pipeline stages execute through child context;
- `src/exec/child_command.c`
  - builtins reached through child execution use `blt_execute()`;
- `src/builtins/cd.c`;
- `src/builtins/export.c`;
- `src/builtins/unset.c`;
- `src/builtins/exit.c`.

The final implementation runs every recognized standalone builtin through the
parent wrapper.

Builtins used as pipeline stages execute inside pipeline children.

### Preserved development material

`docs/history/design/minishell-architecture.md` explicitly distinguishes
builtin execution contexts.

It records that standalone `cd`, `export`, `unset` and `exit` must run in the
parent when their state changes need to persist.

The same material records that parent execution may require temporary
redirections using the sequence:

```text
save stdin/stdout
apply redirections
run builtin
restore stdin/stdout
```

It also records that builtins used as pipeline stages should execute in child
processes so that pipeline data flow remains process-based and state-changing
side effects do not incorrectly mutate the parent shell.

The final implementation preserves these architectural boundaries while using
the parent wrapper for all standalone builtins.

## References

Maintained architecture:

- [`../architecture/runtime-flow.md`](../architecture/runtime-flow.md)
- [`../architecture/process-and-signals.md`](../architecture/process-and-signals.md)
- [`../architecture/resource-ownership.md`](../architecture/resource-ownership.md)

Preserved development material:

- [`../history/design/minishell-architecture.md`](../history/design/minishell-architecture.md)
