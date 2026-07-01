# MSH-14 Mandatory Builtins Test Matrix

This file documents the manual validation matrix for the mandatory builtin implementation.

Related Jira ticket: MSH-14 Mandatory Builtins  
Final validation subtask: MSH-63 Manual builtin test matrix

## Scope

Builtins covered:

| Builtin | Mandatory scope |
| --- | --- |
| echo | echo with -n |
| pwd | no options |
| env | no options or arguments |
| export | no options |
| unset | no options |
| cd | relative or absolute path |
| exit | no options |

Additional checks:

| Area | Covered |
| --- | --- |
| Parent-context execution | yes |
| Redirections with builtins | yes |
| Exit status updates | yes |
| Environment mutation | yes |
| Valgrind | yes |
| Norminette | yes |
| No relink | yes |

---

## Build checks

Run from repository root:

```sh
make fclean && make
make
```

Expected:

```text
First make builds minishell.
Second make prints: Nothing to be done for 'all'.
```

---

## Norminette checks

Run:

```sh
norminette -R CheckForbiddenSourceHeader \
	include/minishell.h \
	src/builtins \
	src/env \
	src/exec \
	src/shell
```

Expected known notices:

```text
INVALID_HEADER
GLOBAL_VAR_DETECTED for the allowed signal global
```

No expected issues:

```text
TOO_MANY_LINES
TOO_MANY_FUNCS
FORBIDDEN_CHAR_NAME
SPC_AFTER_POINTER
```

---

## echo

Start minishell:

```sh
./minishell
```

Inside minishell:

```sh
echo hello
echo hello world
echo -n hello
echo -nnnn hello
echo -nnnnhello
echo-nnnn hello
echo $?
exit
```

Expected behavior:

| Command | Expected |
| --- | --- |
| `echo hello` | prints `hello` with newline |
| `echo hello world` | prints `hello world` |
| `echo -n hello` | prints `hello` without newline |
| `echo -nnnn hello` | treats repeated `n` as valid `-n` |
| `echo -nnnnhello` | prints `-nnnnhello` |
| `echo-nnnn hello` | command not found |
| `echo $?` | status reflects previous command |

---

## pwd

Start minishell:

```sh
./minishell
```

Inside minishell:

```sh
pwd
pwd > pwd.txt
cat pwd.txt
echo $?
exit
```

Expected behavior:

| Command | Expected |
| --- | --- |
| `pwd` | prints current directory |
| `pwd > pwd.txt` | writes current directory to file |
| `cat pwd.txt` | shows same path |
| `echo $?` | prints `0` after successful commands |

Cleanup after leaving minishell:

```sh
rm -f pwd.txt
```

---

## env

Start minishell:

```sh
./minishell
```

Inside minishell:

```sh
env
env > env.txt
cat env.txt
echo $?
exit
```

Expected behavior:

| Command | Expected |
| --- | --- |
| `env` | prints current environment |
| `env > env.txt` | writes environment to file |
| `cat env.txt` | shows environment content |
| `echo $?` | prints `0` |

Cleanup after leaving minishell:

```sh
rm -f env.txt
```

---

## export

Start minishell:

```sh
./minishell
```

Inside minishell:

```sh
export TEST=hello
env
echo $TEST
export TEST=world
echo $TEST
export ONLYNAME
env
export 1BAD=value
echo $?
export A=1 B=2 C=3
echo $A
echo $B
echo $C
export B
echo $B
export A=1 B = 2
echo $A
echo $B
echo $?
export > export.txt
cat export.txt
exit
```

Expected behavior:

| Command | Expected |
| --- | --- |
| `export TEST=hello` | adds `TEST` |
| `echo $TEST` | prints `hello` |
| `export TEST=world` | replaces `TEST` value |
| `echo $TEST` | prints `world` |
| `export ONLYNAME` | adds exported name without value |
| `export 1BAD=value` | prints identifier error |
| `echo $?` | prints `1` |
| `export A=1 B=2 C=3` | adds multiple entries |
| `echo $A` | prints `1` |
| `echo $B` | prints `2` |
| `echo $C` | prints `3` |
| `export B` | does not erase existing `B=2` |
| `export A=1 B = 2` | preserves valid args and errors invalid tokens |
| `export > export.txt` | redirects export output |

Cleanup after leaving minishell:

```sh
rm -f export.txt
```

---

## unset

Start minishell:

```sh
./minishell
```

Inside minishell:

```sh
export TEST=hello A=1 B=2
echo $TEST
unset TEST
echo $TEST
unset A B
echo $A
echo $B
unset DOES_NOT_EXIST
echo $?
unset 1BAD
echo $?
unset A=1
echo $?
exit
```

Expected behavior:

| Command | Expected |
| --- | --- |
| `unset TEST` | removes `TEST` |
| `echo $TEST` | prints empty line |
| `unset A B` | removes multiple variables |
| `unset DOES_NOT_EXIST` | no error |
| `echo $?` | prints `0` |
| `unset 1BAD` | identifier error |
| `echo $?` | prints `1` |
| `unset A=1` | identifier error |
| `echo $?` | prints `1` |

---

## cd

Start minishell:

```sh
./minishell
```

Inside minishell:

```sh
pwd
echo $PWD
echo $OLDPWD
cd ..
pwd
echo $PWD
echo $OLDPWD
cd .
echo $?
cd /does/not/exist
echo $?
cd /tmp
pwd
echo $PWD
echo $OLDPWD
cd
pwd
echo $PWD
echo $?
cd one two
echo $?
exit
```

Expected behavior:

| Command | Expected |
| --- | --- |
| `cd ..` | changes to parent directory |
| `echo $PWD` | matches new current directory |
| `echo $OLDPWD` | matches previous directory |
| `cd .` | returns status `0` |
| `cd /does/not/exist` | prints error and returns `1` |
| `cd /tmp` | changes to `/tmp` |
| `cd` | changes to `$HOME` |
| `cd one two` | prints too many arguments and returns `1` |

---

## exit

Each `exit` case should be tested in a separate shell session.

### exit with no arguments

Run:

```sh
./minishell
```

Inside minishell:

```sh
exit
```

After minishell exits:

```sh
echo $?
```

Expected:

```text
0
```

### exit with numeric status

Run:

```sh
./minishell
```

Inside minishell:

```sh
exit 42
```

After minishell exits:

```sh
echo $?
```

Expected:

```text
42
```

### exit status wrapping

Run:

```sh
./minishell
```

Inside minishell:

```sh
exit 256
```

After minishell exits:

```sh
echo $?
```

Expected:

```text
0
```

Run:

```sh
./minishell
```

Inside minishell:

```sh
exit -1
```

After minishell exits:

```sh
echo $?
```

Expected:

```text
255
```

### numeric argument required

Run:

```sh
./minishell
```

Inside minishell:

```sh
exit abc
```

After minishell exits:

```sh
echo $?
```

Expected:

```text
exit
minishell: exit: abc: numeric argument required
2
```

### too many arguments

Run:

```sh
./minishell
```

Inside minishell:

```sh
exit 1 2
echo $?
pwd
exit 7
```

After minishell exits:

```sh
echo $?
```

Expected:

```text
exit
minishell: exit: too many arguments
1
pwd still works after the failed exit
7
```

---

## Builtins with redirections

Start minishell:

```sh
./minishell
```

Inside minishell:

```sh
echo hello > out.txt
cat out.txt
pwd > pwd.txt
cat pwd.txt
env > env.txt
cat env.txt
export TEST=hello > export_set.txt
cat export_set.txt
export > export.txt
cat export.txt
exit
```

Expected behavior:

| Command | Expected |
| --- | --- |
| `echo hello > out.txt` | file contains `hello` |
| `pwd > pwd.txt` | file contains current directory |
| `env > env.txt` | file contains environment |
| `export TEST=hello > export_set.txt` | assignment succeeds, usually no output |
| `export > export.txt` | file contains export output |

Cleanup after leaving minishell:

```sh
rm -f out.txt pwd.txt env.txt export_set.txt export.txt
```

---

## Error status checks

Start minishell:

```sh
./minishell
```

Inside minishell:

```sh
cat < missing_file
echo $?
export 1BAD=value
echo $?
unset 1BAD
echo $?
cd /does/not/exist
echo $?
cd one two
echo $?
exit
```

Expected behavior:

| Command | Expected status |
| --- | --- |
| `cat < missing_file` | `1` |
| `export 1BAD=value` | `1` |
| `unset 1BAD` | `1` |
| `cd /does/not/exist` | `1` |
| `cd one two` | `1` |

---

## Valgrind checks

Run:

```sh
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./minishell
```

Inside minishell:

```sh
echo hello
pwd
env
export TEST=hello
echo $TEST
export TEST=world
echo $TEST
unset TEST
echo $TEST
cd ..
echo $PWD
echo $OLDPWD
exit 42
```

Expected Valgrind summary:

```text
FILE DESCRIPTORS: 3 open (3 std) at exit

definitely lost: 0 bytes in 0 blocks
indirectly lost: 0 bytes in 0 blocks
possibly lost: 0 bytes in 0 blocks
ERROR SUMMARY: 0 errors from 0 contexts
```

Known acceptable output:

```text
still reachable blocks from readline/libreadline/libtinfo
```

---

## Final validation summary

| Check | Status |
| --- | --- |
| echo | PASS |
| pwd | PASS |
| env | PASS |
| export | PASS |
| unset | PASS |
| cd | PASS |
| exit | PASS |
| redirections with builtins | PASS |
| parent-context builtins | PASS |
| exit status updates | PASS |
| Valgrind definite/indirect/possible leaks | PASS |
| FD leak check | PASS |
| clean build | PASS |
| no relink | PASS |
| Norm known notices only | PASS |
