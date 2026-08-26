# MSH-12 Expansion Manual Tests

## Scope

This file tracks manual checks for mandatory environment and exit status expansion.

Current implementation expands variables while reading unquoted and double-quoted segments, because this is the moment where quote context is still known.

Single-quoted segments are read separately and intentionally do not call expansion, preserving their literal content.

## MSH-46 - Environment variables outside single quotes

| Input                           | Expected behavior                       |
| ------------------------------- | --------------------------------------- |
| `echo $USER`                    | Expands `$USER`                         |
| `echo $HOME`                    | Expands `$HOME`                         |
| `echo $PATH`                    | Expands `$PATH`                         |
| `echo $THIS_VAR_DOES_NOT_EXIST` | Expands to an empty string              |
| `echo hello$USER`               | Expands and joins with surrounding text |
| `echo $USERhello`               | Looks for variable `USERhello`          |

## MSH-47 - Last exit status

| Input                          | Expected behavior                  |
| ------------------------------ | ---------------------------------- |
| `echo $?` after startup        | Prints `0`                         |
| invalid command then `echo $?` | Prints `127`                       |
| `pwd` then `echo $?`           | Prints `0`                         |
| `echo $?text`                  | Expands `$?` and joins with `text` |

## MSH-48 - Single quote literal content

| Input               | Expected behavior             |
| ------------------- | ----------------------------- |
| `echo '$USER'`      | Prints `$USER` literally      |
| `echo '$?'`         | Prints `$?` literally         |
| `echo '$HOME'`      | Prints `$HOME` literally      |
| `echo 'hello$USER'` | Prints `hello$USER` literally |

## MSH-49 - Double quote expansion

| Input                | Expected behavior                                              |
| -------------------- | -------------------------------------------------------------- |
| `echo "$USER"`       | Expands `$USER`                                                |
| `echo "$HOME"`       | Expands `$HOME`                                                |
| `echo "$?"`          | Expands to the last exit status                                |
| `echo "Hello $USER"` | Expands `$USER` inside the quoted string                       |
| `echo "Hello$USER"`  | Expands and joins with surrounding text                        |
| `echo "$USERhello"`  | Looks for variable `USERhello` and expands to empty if missing |

## MSH-50 - Missing variables expand to empty string

| Input                                 | Expected behavior                                             |
| ------------------------------------- | ------------------------------------------------------------- |
| `echo $THIS_VAR_DOES_NOT_EXIST`       | Prints an empty argument/output line                          |
| `echo "$THIS_VAR_DOES_NOT_EXIST"`     | Prints an empty argument/output line                          |
| `echo before$THIS_VAR_DOES_NOT_EXIST` | Prints `before`                                               |
| `echo $THIS_VAR_DOES_NOT_EXISTafter`  | Looks for `THIS_VAR_DOES_NOT_EXISTafter` and expands to empty |
| `echo "$USERhello"`                   | Looks for `USERhello` and expands to empty if missing         |

## MSH-52 - Final Manual Validation

Final validation completed for mandatory expansion behavior.

Checks performed:

* clean build with `make fclean && make`
* unquoted variable expansion
* double quote variable expansion
* single quote literal preservation
* last exit status expansion with `$?`
* missing variable expansion to empty string
* literal `$` behavior
* unclosed quote syntax errors
* shell continuity after syntax errors
* basic Valgrind pass

Validated behavior:

* `$USER` expands outside quotes.
* `$USER` expands inside double quotes.
* `$USER` stays literal inside single quotes.
* `$HOME` expands outside quotes.
* `$HOME` expands inside double quotes.
* `$HOME` stays literal inside single quotes.
* `$?` expands outside quotes.
* `$?` expands inside double quotes.
* `$?` stays literal inside single quotes.
* Missing variables expand to an empty string.
* `$USERhello` is parsed as variable `USERhello`.
* `$` alone stays literal.
* Unclosed quotes return a syntax error and the shell stays interactive.
* Concatenated quoted and unquoted segments still form one word when appropriate.

Valgrind summary:

* no definitely lost leaks
* no indirectly lost leaks
* no possibly lost leaks
* no Valgrind error summary entries

Known notes:

* `still reachable` blocks are expected from `readline`/`libtinfo`.
* Child-process Valgrind output may show inherited reachable memory when external command execution exits through `exit()`.

## Notes

`$USERhello` is parsed as variable `USERhello`, not as `$USER` followed by `hello`.

`${USER}hello` style expansion is not part of the current mandatory scope.

Double quotes expand `$VAR` and `$?`.

Single quotes preserve literal content and never expand `$VAR` or `$?`.

