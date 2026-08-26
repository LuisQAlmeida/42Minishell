# Process and Signal Model

This document describes the process topology and context-dependent signal
behaviour of the maintained Minishell implementation.

For the overall system structure, see
[`system-overview.md`](system-overview.md).

For the complete command lifecycle, see
[`runtime-flow.md`](runtime-flow.md).

## Overview

Minishell maintains one persistent interactive parent process and creates child
processes only when the execution context requires them.

The main runtime contexts are:

```text
interactive parent shell
        |
        +--> standalone builtin
        |       executes in parent
        |
        +--> standalone external command
        |       forks one execution child
        |
        +--> pipeline
        |       forks one child per command stage
        |
        `--> heredoc preparation
                forks one temporary heredoc child
```

Signal handling changes according to these contexts.

## Process Roles

### Interactive parent shell

The original Minishell process owns the persistent session state.

It is responsible for:

- the readline prompt loop;
- the mutable shell environment;
- the previous exit status;
- the session exit flag;
- parsing and command construction;
- creating execution children;
- waiting for foreground work;
- returning to the prompt after execution.

This process survives across command lines until the session ends.

### Standalone builtin execution

A standalone builtin executes directly in the interactive parent process.

The execution wrapper:

1. saves parent `stdin` and `stdout`;
2. applies command redirections;
3. executes the builtin;
4. restores the original standard descriptors.

Running standalone builtins in the parent allows mutations of persistent shell
state to survive the command.

This is important for:

- `cd`;
- `export`;
- `unset`;
- `exit`.

The same parent execution path is also used for non-stateful standalone
builtins such as `echo`, `pwd` and `env`.

### Standalone external command

A standalone external command causes the parent to call `fork()`.

The resulting topology is:

```text
Minishell parent
      |
      +-- fork
           |
           +--> parent
           |      waits
           |
           `--> child
                  applies redirections
                  resolves command
                  execve()
```

The child installs normal child signal behaviour before entering the common
execution path.

If `execve()` succeeds, the child process image is replaced by the target
program.

### Pipeline execution

Each pipeline stage executes in its own child process.

Conceptually:

```text
Minishell parent
      |
      +--> fork stage 1
      |
      +--> fork stage 2
      |
      +--> fork stage 3
      |
      `--> wait for all stages
```

Each child:

1. installs child signal behaviour;
2. connects its pipeline input and output;
3. closes inherited pipeline descriptors;
4. applies command-specific redirections;
5. executes a builtin or external command.

The parent remains responsible for advancing the rolling pipeline descriptor
state and eventually collecting every pipeline child.

### Pipeline builtins

A builtin used as a pipeline stage executes in a pipeline child.

For example:

```text
cd /tmp | cat
```

runs `cd` inside a child process.

Changes to working directory, environment or `should_exit` therefore affect
only that child's inherited process state.

They do not modify the persistent parent shell.

### Heredoc child

Heredoc input is collected by a dedicated temporary child process before the
command that consumes the heredoc is launched.

The topology is:

```text
Minishell parent
      |
      +-- pipe
      |
      +-- fork
           |
           +--> heredoc child
           |      readline("> ")
           |      writes body to pipe
           |      exits
           |
           `--> parent
                  waits
                  retains pipe read end
```

The resulting readable descriptor is stored for later redirection application.

For pipelines, all heredocs are prepared before pipeline command children are
launched.

## Forked State

A child created by `fork()` receives a process-local copy of the parent's
address space and open descriptor table.

This includes references to:

- `t_shell`;
- the token list;
- the command chain;
- the shell environment;
- open file descriptors available at the fork point.

Changes made by a child to ordinary process memory do not update the parent's
copy.

This is why stateful builtins must execute in the parent when their effects are
expected to persist.

## Child Cleanup

Execution children use `exe_child_exit()` when they terminate through Minishell
code.

Before calling `exit()`, it releases the child's process-local copies of:

- the token list;
- the command chain;
- the shell environment.

This cleanup does not release or modify the corresponding objects in the
parent process.

After a successful `execve()`, the process image is replaced and this Minishell
cleanup path is no longer used.

## Signal Contexts

Minishell does not use one signal configuration for the entire program.

Instead, signal behaviour is installed according to the current runtime
context.

The maintained implementation defines four policies:

| Context | `SIGINT` | `SIGQUIT` |
| --- | --- | --- |
| Interactive parent | custom handler | ignored |
| Waiting parent | ignored | ignored |
| Execution child | default | default |
| Heredoc child | custom handler | ignored |

These policies are installed through:

- `sig_set_interactive()`;
- `sig_set_waiting()`;
- `sig_set_child()`;
- `sig_set_heredoc()`.

## Interactive Signal Policy

The interactive parent installs `sig_set_interactive()` while accepting input.

Readline's own signal catching is disabled:

```text
rl_catch_signals = 0
```

so Minishell controls the relevant signal behaviour.

### Interactive `SIGINT`

The custom interactive handler:

1. records the signal in `g_signal`;
2. writes `^C` and a newline;
3. tells readline to begin a new line;
4. replaces the current input buffer with an empty line;
5. redisplays the prompt.

Conceptually:

```text
Ctrl-C
  |
  v
interactive_handler()
  |
  +--> g_signal = SIGINT
  +--> clear current readline input
  `--> redisplay prompt
```

After `readline()` returns control to the session loop,
`ses_handle_signal()` observes the pending value.

It then sets:

```text
shell.last_status = 130
```

and clears `g_signal`.

This makes the interrupted interactive input visible through the next `$?`
expansion.

### Interactive `SIGQUIT`

`SIGQUIT` is ignored while Minishell is at the interactive prompt.

## Waiting Parent Policy

When the parent is waiting for foreground child processes,
`sig_set_waiting()` is installed.

Both relevant signals are ignored by the parent:

```text
SIGINT  -> SIG_IGN
SIGQUIT -> SIG_IGN
```

This policy is used while waiting for:

- a standalone external command;
- pipeline children;
- a heredoc child.

The foreground child processes therefore receive the execution-relevant signal
behaviour without the interactive shell processing the same signal through its
prompt handler.

After normal foreground execution waits complete, the interactive policy is
restored before Minishell returns to the prompt.

## Execution Child Policy

Execution children call `sig_set_child()`.

This restores default behaviour for:

```text
SIGINT  -> SIG_DFL
SIGQUIT -> SIG_DFL
```

The policy applies to:

- standalone external-command children;
- pipeline command children;
- builtins executing as pipeline stages.

An external program launched through `execve()` therefore inherits normal
default behaviour for these signals rather than the interactive shell's custom
handlers.

## Simple Child Wait

For one standalone external child, the parent uses `exe_wait_child()`.

The sequence is:

```text
install waiting policy
        |
        v
waitpid(child)
        |
        v
restore interactive policy
        |
        v
convert wait status
```

If the child exits normally, its exit code becomes the command status.

If it is terminated by signal `N`, the shell status becomes:

```text
128 + N
```

For the relevant interactive signals, the wait path also emits the
corresponding user-visible signal message.

## Pipeline Wait

Pipeline execution waits for all pipeline child processes.

The parent tracks two different pieces of information:

1. the wait status associated with the PID of the final pipeline stage;
2. the first observed `SIGINT` or `SIGQUIT` termination used for the
   user-visible signal message.

The final pipeline status is based on the final stage:

```text
all children reaped
       |
       +--> signal message information
       |
       `--> status belonging to last_pid
                         |
                         v
                 pipeline exit status
```

This preserves the shell convention that a pipeline's result is determined by
its final command while still collecting every child.

## Signal-Derived Status

When a waited process terminates because of a signal, Minishell converts the
result to:

```text
128 + signal_number
```

Therefore:

```text
SIGINT  -> 130
SIGQUIT -> 131
```

for processes terminated by those conventional signal numbers.

## Heredoc Signal Policy

The heredoc reader uses a dedicated signal policy installed by
`sig_set_heredoc()`.

### Heredoc `SIGINT`

The custom heredoc handler:

1. stores `SIGINT` in `g_signal`;
2. writes `^C`;
3. closes the heredoc child's `STDIN_FILENO`.

Closing standard input interrupts the `readline("> ")` input path.

The heredoc read loop then detects the interrupted state and returns failure.

The heredoc child converts that interruption into status `130` before exiting.

### Heredoc `SIGQUIT`

`SIGQUIT` is ignored in the heredoc child.

## Heredoc Parent Wait

The parent side of heredoc setup:

1. closes the pipe write end;
2. installs the waiting signal policy;
3. waits for the heredoc child;
4. converts the heredoc child's status;
5. rejects the prepared heredoc if collection failed;
6. otherwise retains the pipe read end.

The surrounding redirection-preparation path restores the interactive signal
policy after heredoc preparation completes.

## Terminal State Around Heredocs

Before the heredoc child is created, Minishell attempts to save the current
terminal attributes with `tcgetattr()`.

After the heredoc child finishes, the parent restores those attributes with
`tcsetattr()` when a terminal state was successfully captured.

This provides an additional boundary around readline and signal-driven heredoc
interaction.

## `g_signal`

The implementation exposes:

```c
volatile sig_atomic_t g_signal;
```

as the small shared signal-state indicator used by the current process.

Its meaning depends on process context.

In the interactive parent it allows the session loop to observe an interrupt
after the handler has updated readline state.

In the heredoc child it allows the heredoc read loop to distinguish an
interrupt from ordinary end-of-file.

Because processes created by `fork()` have separate address spaces, each
process operates on its own process-local copy after the fork.

## `exit` and Process Context

The `exit` builtin illustrates the importance of process context.

When executed as a standalone builtin:

```text
parent shell
    |
    v
blt_exit()
    |
    +--> shell.should_exit = 1
    |
    v
session loop observes flag
    |
    v
normal cleanup + session exit
```

When executed inside a pipeline:

```text
pipeline child
    |
    v
blt_exit()
    |
    +--> child-local should_exit
    |
    v
exe_child_exit()
```

The persistent parent shell continues running.

## Process and Signal State Transitions

A typical standalone external command follows:

```text
INTERACTIVE PARENT
        |
        | fork
        v
+-----------------------+
|                       |
| parent                | child
|                       |
| waiting signals       | default signals
|                       |
| waitpid()             | execve()
|                       |
+-----------+-----------+
            |
            v
    restore interactive
            |
            v
        next prompt
```

A pipeline follows the same general parent/child separation but creates one
execution child per command stage.

A heredoc introduces a temporary heredoc-specific signal context before command
execution begins.

## Context Summary

```text
PROMPT
  parent
  SIGINT custom
  SIGQUIT ignored
      |
      v
FOREGROUND EXECUTION
  parent: signals ignored
  children: signals default
      |
      v
WAIT COMPLETE
  parent restores interactive signals
      |
      v
PROMPT
```

For heredoc preparation:

```text
HEREDOC SETUP
  parent prepares pipe
      |
      +--> parent: waiting signals
      |
      `--> heredoc child:
             SIGINT custom
             SIGQUIT ignored
      |
      v
HEREDOC COMPLETE
  restore terminal
  restore interactive policy
```

## Architectural Consequences

The process model creates several important behavioural boundaries:

- persistent shell state belongs to the interactive parent;
- child state changes disappear when the child exits;
- standalone builtins can mutate persistent state;
- pipeline builtins cannot mutate the parent shell;
- execution children receive normal signal semantics;
- the parent does not process prompt signals while waiting;
- heredoc input uses a specialized interrupt path;
- foreground process status is converted back into shell-visible status;
- the final pipeline stage determines the pipeline result.

These behaviours are consequences of the maintained process architecture rather
than separate post-processing rules.
