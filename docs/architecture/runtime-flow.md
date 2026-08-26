# Runtime Flow

This document describes the end-to-end runtime flow of one Minishell input line.

It focuses on the order in which the maintained implementation scans input,
expands variables, constructs commands, prepares execution resources, launches
processes and propagates the resulting status back into the persistent shell
session.

For the overall subsystem model, see [`system-overview.md`](system-overview.md).

## End-to-End Command Lifecycle

At the highest level, one interactive command follows this path:

```text
readline()
    |
    v
ses_execute_line()
    |
    v
scn_line()
    |
    +--> quote-aware variable expansion
    |
    v
t_token list
    |
    v
grm_pipeline()
    |
    v
t_cmd chain
    |
    v
execution dispatch
    |
    +--> exe_simple()
    |
    `--> exe_pipeline()
              |
              v
        execution result
              |
              v
      shell.last_status
              |
              v
          next prompt
```

The persistent `t_shell` object survives this entire cycle and is reused for
the next command line.

## 1. Interactive Input

The session loop is implemented by `ses_loop()`.

For each prompt iteration it:

1. reads a line with `readline("minishell$ ")`;
2. processes any interactive `SIGINT` state;
3. exits the loop on end-of-file;
4. skips blank input;
5. adds non-blank input to readline history;
6. calls `ses_execute_line()`;
7. stores the returned result in `shell.last_status`;
8. checks whether a builtin requested session termination.

The input buffer returned by `readline()` is owned by the session loop and is
freed after the line has been processed.

## 2. Line Execution Boundary

`ses_execute_line()` coordinates the main phases for one input line:

```text
input line
    |
    v
scan
    |
    v
grammar
    |
    v
dispatch
    |
    v
cleanup
```

It is also the boundary that converts scanner and grammar failures into shell
status values and user-visible errors.

### Scanner failure

The scanner may report:

- `ERR_UNCLOSED_QUOTE`;
- `ERR_MALLOC`.

An empty result with `ERR_NONE` is treated as a successful no-op with status
`0`.

### Grammar failure

Grammar failures may report:

- `ERR_SYNTAX`;
- `ERR_MALLOC`.

When grammar construction fails, the token list is released before returning.

### Successful execution

When both scanning and grammar construction succeed:

1. commands are dispatched for execution;
2. the returned execution result becomes `shell.last_status`;
3. the command chain is released;
4. the token list is released;
5. the resulting status is returned to the session loop.

## 3. Scanning and Quote-Aware Expansion

`scn_line()` walks the input line from left to right.

It skips shell whitespace and distinguishes between:

- words;
- `|`;
- `<`;
- `>`;
- `<<`;
- `>>`.

Operators are emitted directly as token nodes.

Everything else is assembled through `scn_word()`.

## 4. Word Construction

A word may be built from multiple adjacent segments.

For example, conceptually:

```text
abc"$USER"'xyz'
```

is processed as several segments that are joined into one resulting word.

The scanner distinguishes three segment types.

### Plain segments

Plain unquoted text is extracted and passed through `exp_variables()`.

### Single-quoted segments

Single-quoted text is copied literally.

Variable expansion is not performed.

### Double-quoted segments

Double-quoted text is extracted and passed through `exp_variables()`.

The quote delimiters themselves are not retained in the resulting word.

## 5. Variable Expansion

Expansion happens during word scanning while quote context is still known.

The maintained implementation supports:

- environment variables such as `$HOME`;
- the previous command status through `$?`;
- literal `$` when no valid expansion target follows.

This produces the effective string that is stored in the word token.

The runtime order is therefore:

```text
source text
    |
    v
segment classification
    |
    +--> single quote --> literal text
    |
    +--> plain --------> expansion
    |
    `--> double quote -> expansion
             |
             v
        assembled word
             |
             v
         TOK_WORD
```

Expansion is not a separate phase after grammar construction.

## 6. Token Stream

The scanner produces a linked list of `t_token` nodes.

A command such as:

```text
cat < input.txt | grep foo >> output.txt
```

is represented conceptually as:

```text
WORD(cat)
REDIR_IN
WORD(input.txt)
PIPE
WORD(grep)
WORD(foo)
APPEND
WORD(output.txt)
```

Word strings belong to the token nodes until the token list is destroyed.

## 7. Grammar Construction

`grm_pipeline()` validates the pipe structure and converts the token stream into
a linked command chain.

It rejects:

- a leading pipe;
- a trailing pipe;
- consecutive pipes.

The token stream is then split into command segments at each `TOK_PIPE`.

Each segment becomes one `t_cmd`.

## 8. Command Construction

For each command segment, grammar separates command arguments from
redirections.

Word tokens that are normal command arguments are copied into `argv`.

Redirection operators and their following word tokens are converted into
`t_redir` nodes.

Conceptually:

```text
WORD(cat)
REDIR_IN
WORD(input.txt)
WORD(extra)
```

becomes:

```text
t_cmd
├── argv
│   ├── "cat"
│   └── "extra"
│
└── redirs
    └── TOK_REDIR_IN -> "input.txt"
```

Redirection targets are therefore not included in `argv`.

## 9. Execution Dispatch

After grammar construction, the session counts the command nodes.

```text
command count == 1
        |
        v
    exe_simple()

command count > 1
        |
        v
   exe_pipeline()
```

This is the main execution fork in the architecture.

## 10. Simple Command Flow

`exe_simple()` first creates an execution context and prepares the command's
redirections.

The flow then branches:

```text
exe_simple()
    |
    v
prepare redirections
    |
    +--> argc == 0
    |       |
    |       v
    |   redirection-only execution
    |
    +--> builtin
    |       |
    |       v
    |   execute in parent
    |
    `--> external command
            |
            v
           fork
            |
            v
        child execution
```

## 11. Redirection Preparation for Simple Commands

For a simple command, `exe_redir_prepare()` visits every redirection before the
final execution path is selected.

It may:

- open input files;
- create or truncate output files;
- open append targets;
- collect heredoc input.

The resulting descriptor is stored temporarily in the corresponding
`t_redir.fd`.

This means that redirection side effects and failures occur before choosing
between parent builtin execution and child external execution.

## 12. Redirection-Only Commands

A command segment may contain redirections without an executable command.

In that case:

1. parent `stdin` and `stdout` are saved;
2. the redirections are applied;
3. no command body is executed;
4. the original standard descriptors are restored.

This allows redirection side effects to occur without permanently changing the
interactive shell's standard streams.

## 13. Standalone Builtins

Standalone builtins execute inside the persistent shell process.

The parent execution wrapper performs:

```text
save stdin/stdout
      |
      v
apply redirections
      |
      v
execute builtin
      |
      v
restore stdin/stdout
```

Running the builtin in the parent allows changes to session state to survive.

This matters for commands such as:

- `cd`;
- `export`;
- `unset`;
- `exit`.

The same parent path is also used for non-stateful standalone builtins.

## 14. External Simple Commands

External simple commands are executed through `fork()`.

The child:

1. installs child signal behaviour;
2. applies command redirections;
3. chooses direct-path or `PATH` execution;
4. calls `execve()`.

The parent waits for that child and converts its wait status into the shell
status.

## 15. External Command Resolution

Commands containing `/` are treated as explicit paths.

They are passed directly to `execve()` after directory and execution-error
handling.

Commands without `/` are resolved through the shell-owned `PATH` value.

The lookup process:

1. reads `PATH`;
2. splits it by `:`;
3. builds candidate `directory/command` paths;
4. selects the first executable candidate;
5. passes it to `execve()`.

The execution layer distinguishes command-not-found and
found-but-not-executable outcomes through shell exit statuses.

## 16. Pipeline Preparation

A command chain containing multiple `t_cmd` nodes enters `exe_pipeline()`.

Before any pipeline command child is launched, all heredocs belonging to all
pipeline stages are collected.

The order is:

```text
command chain
      |
      v
prepare every heredoc
      |
      +--> failure or interrupt --> abort pipeline
      |
      v
launch pipeline stages
```

Normal file redirections are not globally pre-opened for the pipeline.

They are handled later in the corresponding execution child.

## 17. Rolling Pipeline Construction

The pipeline executor uses `t_pipe_state` rather than allocating an array of all
pipeline descriptors.

It keeps only:

- the previous stage's readable descriptor;
- the current pipe pair;
- the PID of the most recently launched stage.

For a three-stage pipeline:

```text
command A
    |
    +-- create pipe 1
    +-- fork A
    +-- retain read end of pipe 1
              |
              v
command B
    |
    +-- use previous read end
    +-- create pipe 2
    +-- fork B
    +-- close previous read end in parent
    +-- retain read end of pipe 2
              |
              v
command C
    |
    +-- use previous read end
    +-- no next pipe required
    `-- fork C
```

This keeps the parent pipeline state bounded while stages are launched
incrementally.

## 18. Pipeline Child Wiring

Before executing the command body, each pipeline child connects the required
pipeline endpoints.

If a previous stage exists:

```text
previous pipe read end
        |
       dup2
        |
        v
      STDIN
```

If another stage follows:

```text
current pipe write end
        |
       dup2
        |
        v
      STDOUT
```

After duplicating the relevant descriptors, the child closes the inherited
pipeline descriptors.

It then enters the common `exe_child()` execution path.

## 19. Redirections Override Pipeline Wiring

Pipeline wiring happens before command-specific redirections are applied.

The ordering is:

```text
pipeline stdin/stdout setup
          |
          v
exe_child()
          |
          v
command redirections
          |
          v
builtin or execve
```

As a consequence, a command-level redirection may replace a descriptor
previously connected to a pipeline.

Conceptually:

```text
producer | consumer < file
```

connects the pipeline first, then the input redirection replaces the consumer's
`STDIN` with `file`.

Likewise:

```text
producer > file | consumer
```

connects the producer to the pipe first, then the output redirection replaces
its `STDOUT` with `file`.

## 20. Redirection Ordering

Redirections are processed in their original source order.

For repeated input or output redirections, the most recently processed
descriptor becomes the effective one.

Earlier redirections are still opened before being replaced.

Therefore they may still:

- create or truncate files;
- fail;
- produce other observable filesystem effects.

The effective descriptor is then duplicated onto `STDIN` or `STDOUT`.

## 21. Heredoc Flow

A heredoc is collected through a dedicated process and pipe.

The setup flow is:

```text
create pipe
    |
    v
save terminal state
    |
    v
fork
    |
    +-------------------------+
    |                         |
    v                         v
heredoc child             parent
    |                         |
close read end            close write end
    |                         |
install heredoc signals   wait for child
    |                         |
readline("> ") loop       evaluate status
    |                         |
write body to pipe        retain read end
    |                         |
close write end           restore terminal
    |                         |
exit                       v
                       prepared t_redir.fd
```

The command later consumes the read side of the heredoc pipe as an input
redirection.

## 22. Heredoc Body Semantics

The current heredoc reader writes each collected line directly to the pipe.

It does not call the variable-expansion subsystem.

Therefore the maintained baseline does not expand `$VAR` or `$?` inside heredoc
bodies.

This is an implementation limitation and differs from complete Bash heredoc
behaviour.

## 23. Common Child Execution

Simple external commands and pipeline stages converge on `exe_child()`.

Inside the child:

```text
apply redirections
      |
      +--> no argv
      |       |
      |       v
      |     exit 0
      |
      +--> builtin
      |       |
      |       v
      |   execute builtin
      |       |
      |       v
      |     exit
      |
      +--> direct path
      |       |
      |       v
      |     execve
      |
      `--> PATH lookup
              |
              v
            execve
```

If `execve()` succeeds, the Minishell child image is replaced by the external
program.

If execution fails, the child converts the failure into a shell status, cleans
its inherited Minishell resources and exits.

## 24. Child Cleanup

Execution children inherit the parent's memory through `fork()`.

Before an execution child exits through Minishell code, `exe_child_exit()`
releases its process-local copies of:

- the token list;
- the command chain;
- the shell environment.

This cleanup affects only the child's address space.

The persistent parent retains its own copies until its normal cleanup points.

## 25. Waiting for a Simple Child

For a standalone external command, the parent:

1. installs the waiting signal policy;
2. calls `waitpid()` for the child;
3. restores the interactive signal policy;
4. converts the wait result into a shell status.

Normal child exit produces its exit code.

Signal termination produces:

```text
128 + signal_number
```

## 26. Waiting for a Pipeline

The pipeline parent waits until all child processes have been reaped.

It separately tracks the PID of the final pipeline stage.

The returned pipeline status is therefore based on the last stage, while all
pipeline children are still collected.

Conceptually:

```text
wait all pipeline children
         |
         +--> collect signal information
         |
         `--> identify last_pid status
                       |
                       v
                pipeline status
```

## 27. Status Propagation

The final execution result returns through the same boundary regardless of the
execution path:

```text
execution result
      |
      v
ses_execute_line()
      |
      v
shell.last_status
      |
      v
ses_loop()
```

That value then becomes observable through:

- the next `$?` expansion;
- the shell's eventual return value;
- `exit` without an explicit numeric argument.

## 28. Session Exit

The `exit` builtin does not immediately bypass session cleanup when executed in
the parent shell.

Instead it sets:

```text
shell.should_exit = 1
```

and returns an exit status.

After the line finishes:

```text
ses_execute_line()
      |
      v
ses_loop()
      |
      +--> should_exit == 0 --> next prompt
      |
      `--> should_exit != 0
                 |
                 v
              cleanup
                 |
                 v
              return
```

When `exit` runs inside a pipeline child, the modified `should_exit` belongs
only to that child process and does not terminate the parent shell session.

## Runtime Summary

The complete maintained runtime can be summarized as:

```text
persistent shell state
        |
        v
interactive input
        |
        v
scan + quote-aware expansion
        |
        v
token representation
        |
        v
grammar + command representation
        |
        v
execution dispatch
   +----+---------------------+
   |                          |
   v                          v
simple                    pipeline
   |                          |
   +--> parent builtin        +--> prepare heredocs
   +--> redir-only            +--> rolling pipe launch
   `--> child command         `--> child commands
            |                          |
            +------------+-------------+
                         |
                         v
                 wait / collect status
                         |
                         v
                  shell.last_status
                         |
                         v
                     next prompt
```

This flow describes the current maintained implementation.

Earlier development-stage runtime descriptions remain preserved under
[`../history/design/`](../history/design/) for historical context.
