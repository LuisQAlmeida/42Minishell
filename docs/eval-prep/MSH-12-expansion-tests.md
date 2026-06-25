# MSH-12 Expansion Manual Tests

## Scope

This file tracks manual checks for mandatory environment and exit status expansion.

Current implementation expands variables while reading unquoted segments, because this is the moment where quote context is still known.

## MSH-46 - Environment variables outside single quotes

| Input | Expected behavior |
|---|---|
| `echo $USER` | Expands `$USER` |
| `echo $HOME` | Expands `$HOME` |
| `echo $PATH` | Expands `$PATH` |
| `echo $THIS_VAR_DOES_NOT_EXIST` | Expands to an empty string |
| `echo hello$USER` | Expands and joins with surrounding text |
| `echo $USERhello` | Looks for variable `USERhello` |

## MSH-47 - Last exit status

| Input | Expected behavior |
|---|---|
| `echo $?` after startup | Prints `0` |
| invalid command then `echo $?` | Prints `127` |
| `pwd` then `echo $?` | Prints `0` |
| `echo $?text` | Expands `$?` and joins with `text` |

## MSH-48 - Single quote literal content

| Input | Expected behavior |
|---|---|
| `echo '$USER'` | Prints `$USER` literally |
| `echo '$?'` | Prints `$?` literally |
| `echo '$HOME'` | Prints `$HOME` literally |
| `echo 'hello$USER'` | Prints `hello$USER` literally |

## Notes

Double quote expansion is intentionally deferred to MSH-49.

At this stage:

| Input | Current expected behavior |
|---|---|
| `echo "$USER"` | Still prints `$USER` literally |
| `echo "$?"` | Still prints `$?` literally |
