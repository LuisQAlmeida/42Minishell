# MSH-12 Expansion Flow

## Goal

Document where mandatory expansion happens in the current Minishell execution flow.

This file explains how MSH-12 integrates environment variable expansion and last exit status expansion before command parsing and execution.

## Current Flow

main_loop()
  -> run_once(line, &shell)
      -> tokenize_line(line, shell, &err)
          -> read_word()
              -> read_plain_segment()
                  -> expand_env_vars()
              -> read_double_quoted()
                  -> expand_env_vars()
              -> read_single_quoted()
                  -> no expansion
      -> parse_simple_cmd(tokens, &err)
      -> exec_simple_cmd(cmd, shell->envp)

## Design Decision

Expansion is performed during tokenization, while quote context is still available.

This is important because the tokenizer removes quote characters. If expansion happened after tokenization, the shell would no longer know whether a piece of text originally came from:

- unquoted input
- single-quoted input
- double-quoted input

By expanding during segment reading, the shell can apply the correct behavior before quote context disappears.

## Quote Behavior

| Context | Expansion behavior |
|---|---|
| Unquoted text | Expands `$VAR` and `$?` |
| Single quotes | Preserves literal content |
| Double quotes | Expands `$VAR` and `$?` |

## Integration Point

Expansion happens before command parsing and execution.

This means `parse_simple_cmd()` receives already-expanded `TOK_WORD` values and only builds `argv`.

`exec_simple_cmd()` does not perform expansion. It only executes the command represented by `argv`.

## Implemented Expansion Rules

### Environment Variables

The current implementation expands environment variables in unquoted and double-quoted segments.

Examples:

- `echo $USER`
- `echo "$USER"`
- `echo hello$USER`
- `echo "hello$USER"`

### Last Exit Status

The current implementation expands `$?` to the shell's last exit status.

Examples:

- `echo $?`
- `invalid_command`
- `echo $?`
- `echo "$?"`

### Single Quotes

Single quotes preserve literal content.

Examples:

- `echo '$USER'`
- `echo '$?'`
- `echo 'hello$USER'`

These inputs should not expand `$USER` or `$?`.

### Missing Variables

Missing variables expand to an empty string.

Examples:

- `echo $THIS_VAR_DOES_NOT_EXIST`
- `echo "$THIS_VAR_DOES_NOT_EXIST"`
- `echo "$USERhello"`

The input `$USERhello` is parsed as a lookup for the variable `USERhello`, not as `$USER` followed by `hello`.

## Data Ownership

Expansion functions return newly allocated strings.

The caller is responsible for freeing temporary strings after they are joined into the final token value.

Current ownership flow:

read_plain_segment()
  -> ft_substr()
  -> expand_env_vars()
  -> free(original segment)
  -> return(expanded segment)

read_double_quoted()
  -> ft_substr()
  -> expand_env_vars()
  -> free(original segment)
  -> return(expanded segment)

read_single_quoted()
  -> ft_substr()
  -> return(literal segment)

## Error Handling

If memory allocation fails during expansion:

- `ERR_MALLOC` is set.
- The current command is not executed.
- Control returns to the prompt.

If quotes are not closed:

- `ERR_UNCLOSED_QUOTE` is set.
- The current command is not executed.
- Control returns to the prompt.

## Current Limitations

The following are intentionally outside the current mandatory expansion scope:

- `${VAR}` syntax
- command substitution
- arithmetic expansion
- wildcard expansion
- field splitting beyond the current tokenizer behavior
- special parameters other than `$?`
- positional parameters such as `$1`, `$2`, `$3`

## Review Checklist

Use this checklist to verify the integration behavior:

- `echo $USER`
- `echo "$USER"`
- `echo '$USER'`
- `echo $?`
- `invalid_command`
- `echo $?`
- `echo "$?"`
- `echo '$?'`
- `echo $THIS_VAR_DOES_NOT_EXIST`
- `echo "$THIS_VAR_DOES_NOT_EXIST"`
- `echo hello$USER`
- `echo "hello$USER"`
- `echo $USERhello`
- `echo "$USERhello"`

Expected summary:

- `$USER` expands outside quotes.
- `$USER` expands inside double quotes.
- `$USER` stays literal inside single quotes.
- `$?` expands outside quotes.
- `$?` expands inside double quotes.
- `$?` stays literal inside single quotes.
- Missing variables expand to empty string.
- Expansion happens before `parse_simple_cmd()`.
- `exec_simple_cmd()` receives already-expanded `argv`.
