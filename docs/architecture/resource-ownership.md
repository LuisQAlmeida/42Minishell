# Resource Ownership

This document describes ownership and lifetime boundaries for memory, file
descriptors and process-local resources in the maintained Minishell
implementation.

The goal is to make resource transfer and cleanup responsibilities explicit.

For the overall subsystem model, see
[`system-overview.md`](system-overview.md).

For execution ordering, see
[`runtime-flow.md`](runtime-flow.md).

For process boundaries and signal behaviour, see
[`process-and-signals.md`](process-and-signals.md).

## Ownership Model

Minishell operates with several different resource lifetimes:

```text
session lifetime
    |
    +--> t_shell
    `--> shell-owned environment

input-line lifetime
    |
    +--> readline buffer
    +--> token list
    `--> command chain

execution lifetime
    |
    +--> redirection descriptors
    +--> pipeline descriptors
    +--> heredoc descriptors
    `--> temporary stdio backups

process-local lifetime
    |
    `--> resources inherited through fork()
```

The implementation relies on clear transitions between these lifetimes rather
than one global cleanup owner.

## Session-Owned State

The interactive session owns one `t_shell` structure for the duration of
`ses_loop()`.

```c
typedef struct s_shell
{
    char    **envp;
    int     last_status;
    int     should_exit;
}   t_shell;
```

The structure itself is stack-allocated inside the session loop.

Its dynamically allocated environment is initialized once and released when the
session ends.

## Environment Ownership

At startup, `sta_env_copy()` creates a deep copy of the `envp` received by
`main()`.

Conceptually:

```text
process envp
    |
    | duplicate array
    | duplicate each entry
    v
shell.envp
```

From that point onward, the persistent shell owns its environment copy.

The original `envp` supplied by the process is not mutated by Minishell.

### Environment lookup

`sta_env_value()` returns a pointer into an existing environment entry.

It does not allocate a new string and does not transfer ownership.

Conceptually:

```text
shell.envp[i]
"HOME=/home/user"
      |
      +--> returned pointer
           "/home/user"
```

Callers must not free that returned pointer.

When expansion requires an owned result, `exp_env_value()` duplicates the
value.

### Replacing an environment entry

When an existing entry is replaced, the state subsystem:

```text
old entry string
      |
      v
free()
      |
      v
new duplicated string
      |
      v
same envp slot
```

The outer environment array remains owned by the shell.

### Extending the environment

When a new variable is added:

```text
old char ** array
      |
      | transfer existing char * entries
      v
new char ** array
      |
      +--> existing strings
      +--> duplicated new entry
      `--> NULL
```

The old pointer array is released after its entries have been transferred.

The strings themselves are not duplicated during that array transfer.

### Removing an environment entry

When an entry is removed:

```text
old environment
      |
      +--> removed string -> free()
      |
      `--> retained strings
              |
              | transfer
              v
          new char ** array
```

The old pointer array is then released.

This preserves ownership of retained strings while freeing the removed entry.

## Readline Input Ownership

Each call to `readline()` returns a dynamically allocated line buffer.

The session loop owns that buffer.

```text
readline()
    |
    v
char *line
    |
    +--> scanner reads it
    |
    +--> history may copy/use it
    |
    v
free(line)
```

The line remains valid throughout `ses_execute_line()` and is released after
that call returns.

The scanner does not take ownership of the original line.

## Token Ownership

The scanner creates a linked list of `t_token` objects.

```c
typedef struct s_token
{
    t_toktype       type;
    char            *value;
    struct s_token  *next;
}   t_token;
```

Each token node owns itself.

A `TOK_WORD` token also owns its `value`.

Operator tokens normally have a null value.

### Word construction

Word scanning creates temporary segment strings.

Conceptually:

```text
source line
    |
    v
segment allocation
    |
    v
optional expansion
    |
    v
append into assembled word
    |
    v
TOK_WORD.value
```

Temporary segment allocations are released as the word is assembled.

The final assembled word is transferred to the token created by
`scn_emit_word()`.

### Token cleanup

`scn_token_clear()` owns destruction of a complete token list.

For every node it releases:

```text
token.value
    |
    v
token node
```

The list can therefore be destroyed independently of the command model created
later by grammar.

## Command Ownership

The grammar subsystem builds a separate linked command representation.

```c
typedef struct s_cmd
{
    char            **argv;
    int             argc;
    t_redir         *redirs;
    struct s_cmd    *next;
}   t_cmd;
```

A command node owns:

```text
t_cmd
├── argv array
├── argv strings
├── redirection list
└── command node itself
```

Commands do not borrow argument strings directly from tokens.

## Token-to-Command Copy Boundary

Grammar duplicates strings when constructing commands.

For a normal word:

```text
t_token.value
      |
      | ft_strdup()
      v
cmd->argv[i]
```

For a redirection target:

```text
t_token.value
      |
      | ft_strdup()
      v
t_redir.target
```

This creates two independent ownership domains:

```text
scanner ownership
    t_token + token.value

grammar ownership
    t_cmd + argv + t_redir
```

The token list and command chain can therefore be released independently.

## Redirection Ownership

Each redirection node is represented by:

```c
typedef struct s_redir
{
    t_toktype       type;
    char            *target;
    int             fd;
    struct s_redir  *next;
}   t_redir;
```

A redirection owns:

```text
redirection node
redirection target string
optional prepared descriptor
```

The descriptor field starts at:

```text
fd = -1
```

meaning no prepared descriptor is currently owned.

## Prepared Redirection Descriptor

During execution preparation, a redirection may acquire an open descriptor.

For example:

```text
open(target)
    |
    v
fd
    |
    v
r->fd
```

At that point, the `t_redir` node owns the descriptor until it is either:

```text
consumed during application
replaced
or closed during cleanup
```

### Replacement during preparation

If a redirection already owns a prepared descriptor and a new descriptor is
assigned:

```text
old r->fd
    |
    v
close()
    |
    v
new r->fd
```

This prevents the redirection node from retaining multiple prepared
descriptors.

## Redirection Descriptor Transfer

`exe_redir_one()` transfers a prepared descriptor out of the redirection node.

Conceptually:

```text
r->fd
  |
  | move
  v
local fd

r->fd = -1
```

After this point the redirection node no longer owns that descriptor.

The execution path becomes responsible for it.

## Effective Input and Output Descriptors

While redirections are applied, execution tracks at most one effective input
descriptor and one effective output descriptor:

```text
in_fd
out_fd
```

When another redirection of the same direction is encountered, the previous
effective descriptor is closed.

Example:

```text
< first < second

open first
    |
    v
in_fd = first

open second
    |
    +--> close first
    |
    v
in_fd = second
```

The same rule applies to output redirections.

This preserves source-order side effects while making the latest successfully
processed descriptor the effective endpoint.

## Applying Standard Descriptors

After all redirections have been processed, effective descriptors are applied
through `dup2()`.

Conceptually:

```text
effective input fd
        |
       dup2
        |
        v
      STDIN
        |
        v
close original input fd
```

and:

```text
effective output fd
        |
       dup2
        |
        v
      STDOUT
        |
        v
close original output fd
```

The duplicated standard descriptor remains open as part of the process
descriptor table.

The original source descriptor is closed after duplication.

## Redirection Cleanup Fallback

If a command is destroyed while a redirection still has:

```text
r->fd != -1
```

`grm_redir_clear()` closes that descriptor.

This provides cleanup for prepared resources that were not transferred to the
normal application path.

It then releases the redirection target and node.

## Simple Command Preparation

For a simple command, all redirection types are prepared before the execution
path is selected.

```text
exe_simple()
    |
    v
exe_redir_prepare()
    |
    +--> input files
    +--> output files
    +--> append files
    `--> heredocs
```

Prepared descriptors are stored temporarily in the corresponding `t_redir`
nodes.

They are later consumed by:

```text
redirection-only execution
standalone builtin execution
or child execution
```

### Prepared descriptors across `fork()`

For an external simple command, redirection preparation occurs before the
execution child is created.

After `fork()`, parent and child have process-local descriptor-table entries
referring to the prepared resources.

The child may transfer its copy through redirection application:

```text
child t_redir.fd
        |
        v
effective fd
        |
       dup2
        |
        v
STDIN / STDOUT
```

This does not modify the parent's `t_redir.fd`, because the processes have
separate memory and descriptor tables after the fork.

The parent retains its own prepared descriptor until the per-line command model
is destroyed, at which point `grm_redir_clear()` closes any descriptor still
owned by the parent-side redirection node.

## Pipeline Redirection Preparation

Pipeline preparation treats heredocs differently from normal file
redirections.

Before pipeline command children are launched:

```text
all pipeline heredocs
        |
        v
prepared in parent context
```

Their readable descriptors are stored in `t_redir.fd`.

Normal file redirections are opened later by the individual pipeline children
during redirection application.

This avoids globally pre-opening every ordinary pipeline file redirection.

## Parent Standard-Stream Backups

Some operations must temporarily redirect the persistent parent shell.

These include:

```text
standalone builtins
redirection-only commands
```

Before modifying the parent's standard descriptors:

```text
STDIN  -> dup() -> stdin_save
STDOUT -> dup() -> stdout_save
```

The saved descriptors are owned by the current parent-execution scope.

After the operation:

```text
stdin_save
    |
   dup2
    |
    v
STDIN

stdout_save
    |
   dup2
    |
    v
STDOUT
```

The backup descriptors are then closed.

This prevents command redirections from leaking into later prompt iterations.

## Pipeline Descriptor Ownership

Pipeline construction uses:

```c
typedef struct s_pipe_state
{
    int     prev_fd;
    int     pipefd[2];
    pid_t   last_pid;
}   t_pipe_state;
```

The parent uses this state to manage only the descriptors needed for the current
and next pipeline stage.

## `prev_fd`

`prev_fd` represents the readable endpoint inherited from the previous pipeline
stage.

While the next child is being created, the parent owns this descriptor.

After the child has inherited the descriptor:

```text
old prev_fd
      |
      v
close in parent
```

The new pipe's read end then becomes the next `prev_fd`.

## Current Pipe Pair

When another stage follows, the parent creates:

```text
pipefd[0]  read end
pipefd[1]  write end
```

Both descriptors initially belong to the current process descriptor table.

After `fork()`, both parent and child have inherited copies.

Ownership responsibilities then diverge by process.

## Pipeline Child Descriptor Setup

The child uses the previous read descriptor when input from an earlier stage is
required:

```text
prev_fd
   |
  dup2
   |
   v
STDIN
```

When another stage follows, it uses the current write descriptor:

```text
pipefd[1]
    |
   dup2
    |
    v
STDOUT
```

After those duplicates are established, the child closes the original inherited
pipeline descriptors.

The standard descriptors remain as the active communication endpoints.

## Pipeline Parent Descriptor Progression

After each stage is forked, the parent:

```text
close previous prev_fd
close current write end
retain current read end
```

The retained read end becomes the input candidate for the next stage.

For the last command there is no next pipe to retain.

The result is a rolling descriptor lifecycle:

```text
pipe 1 read
    |
    v
prev_fd
    |
    | next stage launched
    v
close
        pipe 2 read
             |
             v
          prev_fd
             |
             v
           close
```

## Pipeline Failure Cleanup

If pipe creation or forking fails, `pipeline_fail()` closes descriptors still
tracked by `t_pipe_state`.

If at least one child has already been launched, the implementation also waits
through the existing pipeline wait path before returning failure.

This prevents the parent from simply abandoning tracked pipeline resources.

## Heredoc Pipe Ownership

Each heredoc begins with a new pipe:

```text
pipefd[0] = read end
pipefd[1] = write end
```

After `fork()`, parent and heredoc child each inherit their own descriptor-table
entries referring to those pipe endpoints.

## Heredoc Child

The heredoc child does not consume the read end.

It therefore closes:

```text
pipefd[0]
```

and writes collected lines through:

```text
pipefd[1]
```

When collection completes or is interrupted, it closes the write end before
terminating.

Closing the writer is necessary for the future reader to observe end-of-file
after buffered heredoc data has been consumed.

## Heredoc Parent

The parent does not write the heredoc body.

It closes:

```text
pipefd[1]
```

before waiting for the heredoc child.

If heredoc collection succeeds, the parent retains:

```text
pipefd[0]
```

and transfers that readable descriptor into:

```text
t_redir.fd
```

Ownership therefore becomes:

```text
heredoc pipe read end
        |
        v
t_redir.fd
        |
        v
redirection application
        |
        v
STDIN
```

If heredoc collection fails, the parent closes the read end instead.

## Heredoc Descriptor Consumption

When the heredoc redirection is eventually applied:

```text
r->fd
   |
   | transfer
   v
in_fd
   |
  dup2
   |
   v
STDIN
   |
   v
close original heredoc read fd
```

The command then reads heredoc content through its standard input.

## Heredoc Terminal State

Heredoc setup may also temporarily own a copy of terminal attributes:

```text
struct termios saved
```

This object is stack storage rather than heap memory.

When `tcgetattr()` succeeds, the caller records that the saved state is valid.

After the heredoc child finishes, the parent restores the terminal attributes
with `tcsetattr()`.

The lifetime is limited to one heredoc setup operation.

## Child Process Memory

`fork()` gives the child a process-local copy of the parent's memory state.

Conceptually:

```text
parent
├── shell
├── environment
├── tokens
└── commands
      |
      | fork
      v
child-local copies
├── shell
├── environment
├── tokens
└── commands
```

The physical implementation may use copy-on-write internally, but ownership
from Minishell's perspective becomes process-local after the fork.

A child can therefore release its inherited Minishell resources without
destroying the parent's corresponding objects.

## `t_child_ctx`

Execution passes a small context:

```c
typedef struct s_child_ctx
{
    t_cmd       *cmds;
    t_token     *tokens;
    t_shell     *shell;
}   t_child_ctx;
```

The structure carries references used during execution and child cleanup.

It does not create another ownership copy by itself.

Its pointers refer to resources already owned by the surrounding process.

## Child Exit Cleanup

When a child exits through Minishell code, `exe_child_exit()` releases its
process-local inherited resources:

```text
tokens
    |
    v
scn_token_clear()

commands
    |
    v
grm_clear()

shell.envp
    |
    v
sup_free_array()

process
    |
    v
exit(status)
```

The `t_shell` structure itself is not heap-allocated and therefore is not freed.

After `fork()`, this cleanup affects only the child's process-local memory.

## Successful `execve()`

A successful `execve()` replaces the current child process image.

In that case Minishell does not return to `exe_child()` and does not execute
`exe_child_exit()`.

The operating system replaces the process memory image as part of `execve()`.

Descriptors not intentionally closed before `execve()` remain subject to normal
process descriptor semantics.

The execution path therefore closes or redirects pipeline and command
descriptors before attempting the external program.

## Per-Line Parent Cleanup

After execution returns to `ses_execute_line()`, the parent releases:

```text
command chain
    |
    v
grm_clear()

token list
    |
    v
scn_token_clear()
```

The original readline buffer is then released by `ses_loop()`.

The persistent environment is intentionally retained for the next prompt.

## Session Cleanup

When the interactive session ends:

```text
readline history
    |
    v
rl_clear_history()

shell environment
    |
    v
sup_free_array(shell.envp)
```

The session then returns its final status.

## Failure Boundaries

Resource cleanup is distributed according to the layer that acquired the
resource.

Examples include:

```text
scanner allocation failure
    -> scanner clears partial token list

grammar construction failure
    -> grammar clears partial command structures

redirection application failure
    -> execution closes effective redirection descriptors

pipeline setup failure
    -> pipeline closes tracked pipe descriptors

heredoc failure
    -> heredoc parent closes retained pipe endpoint

child execution failure
    -> child-local cleanup before exit
```

This keeps cleanup responsibilities close to the operations that understand the
resource state.

## Ownership Summary

| Resource | Primary owner | Typical release point |
| --- | --- | --- |
| `t_shell` structure | session stack | return from `ses_loop()` |
| `shell.envp` | interactive session | `ses_cleanup()` |
| readline input buffer | prompt iteration | after `ses_execute_line()` |
| `t_token` list | per-line scanner model | `scn_token_clear()` |
| token values | owning token | `scn_token_clear()` |
| `t_cmd` chain | per-line grammar model | `grm_clear()` |
| command `argv` | owning command | `grm_clear()` |
| redirection target | owning `t_redir` | `grm_redir_clear()` |
| prepared `t_redir.fd` | redirection node | transfer or redirection cleanup |
| effective `in_fd` / `out_fd` | redirection application | after `dup2()` or failure |
| parent stdio backups | parent execution scope | after restore |
| pipeline `prev_fd` | pipeline parent | after next stage is launched |
| current pipeline pair | parent and child after `fork()` | context-specific close |
| heredoc write end | heredoc child | heredoc completion |
| heredoc read end | parent, then `t_redir` | redirection application |
| child-local inherited models | execution child | `exe_child_exit()` |
| saved heredoc terminal state | heredoc setup stack | end of setup |

## Ownership Transfers

The most important transfer paths can be summarized as:

```text
scanner word
    |
    v
t_token.value
```

```text
t_token.value
    |
    | duplicate
    v
cmd->argv / redir->target
```

```text
open redirection fd
    |
    v
t_redir.fd
    |
    | transfer
    v
effective in_fd/out_fd
    |
    | dup2
    v
STDIN/STDOUT
```

```text
heredoc pipe read end
    |
    v
t_redir.fd
    |
    v
STDIN
```

```text
pipeline read end
    |
    v
parent prev_fd
    |
    | inherited through fork
    v
child
    |
    | dup2
    v
STDIN
```

These transitions are the main points where responsibility for cleanup changes.

## Architectural Consequences

The ownership model supports several important properties of the maintained
implementation:

- persistent session state is isolated from per-line parsing state;
- token and command representations can be destroyed independently;
- parent-process redirections are temporary and reversible;
- prepared redirection descriptors have an explicit fallback cleanup owner;
- pipeline descriptors are closed progressively instead of accumulated in one
  long-lived descriptor array;
- heredoc descriptors move through an explicit producer-to-redirection
  lifecycle;
- child processes can clean inherited Minishell state independently;
- environment mutations remain under the persistent shell's ownership.

This document describes ownership as implemented in the current source tree.

It does not claim that every future extension should preserve the same resource
model.
