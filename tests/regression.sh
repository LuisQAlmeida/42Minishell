#!/usr/bin/env bash

set -u
set -o pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
MINISHELL="$ROOT_DIR/minishell"

if [ ! -x "$MINISHELL" ]; then
    echo "ERROR: $MINISHELL is not executable."
    echo "Run 'make' before invoking this script directly."
    exit 2
fi

TMP_ROOT="$(mktemp -d "${TMPDIR:-/tmp}/minishell-regression.XXXXXX")"
WORK_DIR="$TMP_ROOT/work"
HOME_DIR="$TMP_ROOT/home"

mkdir -p "$WORK_DIR" "$HOME_DIR"

cleanup()
{
    rm -rf "$TMP_ROOT"
}

trap cleanup EXIT HUP INT TERM

TOTAL=0
PASSED=0
FAILED=0

normalize_stdout()
{
    sed -E \
        -e '/^minishell\$ /d' \
        -e 's/minishell\$ $//' \
        -e '/^> /d' \
        "$1"
}

run_case()
{
    local name="$1"
    local input="$2"
    local expected_stdout="$3"
    local expected_stderr="$4"
    local expected_status="$5"

    local input_file
    local raw_stdout
    local actual_stdout
    local actual_stderr
    local expected_stdout_file
    local expected_stderr_file
    local status

    TOTAL=$((TOTAL + 1))

    rm -rf "$WORK_DIR"
    mkdir -p "$WORK_DIR"

    input_file="$TMP_ROOT/input_$TOTAL"
    raw_stdout="$TMP_ROOT/raw_stdout_$TOTAL"
    actual_stdout="$TMP_ROOT/stdout_$TOTAL"
    actual_stderr="$TMP_ROOT/stderr_$TOTAL"
    expected_stdout_file="$TMP_ROOT/expected_stdout_$TOTAL"
    expected_stderr_file="$TMP_ROOT/expected_stderr_$TOTAL"

    printf '%s' "$input" > "$input_file"
    printf '%s' "$expected_stdout" > "$expected_stdout_file"
    printf '%s' "$expected_stderr" > "$expected_stderr_file"

    (
        cd "$WORK_DIR" || exit 125

        env -i \
            PATH="/usr/bin:/bin" \
            HOME="$HOME_DIR" \
            TERM="dumb" \
            LC_ALL="C" \
            MSH_TEST_VAR="regression_value" \
            "$MINISHELL" \
            < "$input_file" \
            > "$raw_stdout" \
            2> "$actual_stderr"
    )

    status=$?

    normalize_stdout "$raw_stdout" > "$actual_stdout"

    if cmp -s "$expected_stdout_file" "$actual_stdout" \
        && cmp -s "$expected_stderr_file" "$actual_stderr" \
        && [ "$status" -eq "$expected_status" ]
    then
        printf 'PASS  %02d  %s\n' "$TOTAL" "$name"
        PASSED=$((PASSED + 1))
        return
    fi

    printf 'FAIL  %02d  %s\n' "$TOTAL" "$name"
    FAILED=$((FAILED + 1))

    if ! cmp -s "$expected_stdout_file" "$actual_stdout"; then
        printf '\n  stdout differs:\n'
        diff -u \
            --label expected-stdout \
            --label actual-stdout \
            "$expected_stdout_file" \
            "$actual_stdout" || true
    fi

    if ! cmp -s "$expected_stderr_file" "$actual_stderr"; then
        printf '\n  stderr differs:\n'
        diff -u \
            --label expected-stderr \
            --label actual-stderr \
            "$expected_stderr_file" \
            "$actual_stderr" || true
    fi

    if [ "$status" -ne "$expected_status" ]; then
        printf '\n  status differs:\n'
        printf '    expected: %s\n' "$expected_status"
        printf '    actual:   %s\n' "$status"
    fi

    printf '\n'
}

printf 'Minishell behavioural regression suite\n'
printf '======================================\n\n'

# Builtins and external execution

run_case \
    'echo prints an argument with newline' \
    $'echo hello\n' \
    $'hello\n' \
    '' \
    0

run_case \
    'echo -n suppresses trailing newline' \
    $'echo -n hello\n' \
    'hello' \
    '' \
    0

run_case \
    'pwd reports the current working directory' \
    $'pwd\n' \
    "$WORK_DIR"$'\n' \
    '' \
    0

run_case \
    'absolute external command executes' \
    $'/bin/echo external-ok\n' \
    $'external-ok\n' \
    '' \
    0

run_case \
    'PATH resolution executes true' \
    $'true\n' \
    '' \
    '' \
    0

# Quotes and expansion

run_case \
    'single quotes suppress variable expansion' \
    $'echo \'$MSH_TEST_VAR\'\n' \
    '$MSH_TEST_VAR'$'\n' \
    '' \
    0

run_case \
    'double quotes allow variable expansion' \
    $'echo "$MSH_TEST_VAR"\n' \
    $'regression_value\n' \
    '' \
    0

run_case \
    'unquoted variable expands' \
    $'echo $MSH_TEST_VAR\n' \
    $'regression_value\n' \
    '' \
    0

run_case \
    'quoted and literal segments concatenate' \
    $'echo pre"$MSH_TEST_VAR"post\n' \
    $'preregression_valuepost\n' \
    '' \
    0

run_case \
    'missing variable expands to empty' \
    $'echo X"$MSH_MISSING_VAR"Y\n' \
    $'XY\n' \
    '' \
    0

# Exit status and failure propagation

run_case \
    'success and failure update $?' \
    $'true\necho $?\nfalse\necho $?\n' \
    $'0\n1\n' \
    '' \
    0

run_case \
    'command-not-found returns 127' \
    $'this_command_should_not_exist\n' \
    '' \
    $'minishell: this_command_should_not_exist: command not found\n' \
    127

run_case \
    'failing cd updates $?' \
    $'cd /definitely/not/a/real/directory\necho $?\n' \
    $'1\n' \
    $'minishell: cd: /definitely/not/a/real/directory: No such file or directory\n' \
    0

# Stateful builtins

run_case \
    'standalone cd mutates parent shell state' \
    $'cd /tmp\npwd\n' \
    $'/tmp\n' \
    '' \
    0

run_case \
    'export persists in parent shell environment' \
    $'export MSH_LOCAL=42\necho "$MSH_LOCAL"\n' \
    $'42\n' \
    '' \
    0

run_case \
    'unset removes a parent-shell variable' \
    $'export MSH_LOCAL=42\nunset MSH_LOCAL\necho X"$MSH_LOCAL"Y\n' \
    $'XY\n' \
    '' \
    0

run_case \
    'env exposes exported variables' \
    $'export MSH_LOCAL=42\nenv | grep \'^MSH_LOCAL=42$\'\n' \
    $'MSH_LOCAL=42\n' \
    '' \
    0

# Syntax recovery

run_case \
    'missing command after pipe reports error and recovers' \
    $'echo broken |\necho $?\necho recovered\n' \
    $'1\nrecovered\n' \
    $'minishell: syntax error near unexpected token\n' \
    0

run_case \
    'unclosed quote reports error and recovers' \
    $'echo "broken\necho $?\necho recovered\n' \
    $'1\nrecovered\n' \
    $'minishell: syntax error: unclosed quote\n' \
    0

# Pipelines

run_case \
    'simple pipeline transfers stdout' \
    $'echo hello | cat\n' \
    $'hello\n' \
    '' \
    0

run_case \
    'pipeline supports external transformation' \
    $'echo hello | tr a-z A-Z\n' \
    $'HELLO\n' \
    '' \
    0

run_case \
    'pipeline status follows final stage' \
    $'false | true\necho $?\ntrue | false\necho $?\n' \
    $'0\n1\n' \
    '' \
    0

run_case \
    'stateful builtin in pipeline does not mutate parent' \
    $'cd /tmp | cat\npwd\n' \
    "$WORK_DIR"$'\n' \
    '' \
    0

# Redirections

run_case \
    'truncate append and input redirection compose' \
    $'echo first > out.txt\necho second >> out.txt\ncat < out.txt\n' \
    $'first\nsecond\n' \
    '' \
    0

run_case \
    'last output redirection is effective' \
    $'echo final > a.txt > b.txt\ncat a.txt\ncat b.txt\n' \
    $'final\n' \
    '' \
    0

run_case \
    'input redirection failure updates $?' \
    $'cat < file_that_does_not_exist\necho $?\n' \
    $'1\n' \
    $'file_that_does_not_exist: No such file or directory\n' \
    0

run_case \
    'builtin redirection restores parent stdout' \
    $'echo redirected > builtin_out.txt\necho visible\ncat builtin_out.txt\n' \
    $'visible\nredirected\n' \
    '' \
    0

# Heredoc

run_case \
    'basic heredoc supplies command input' \
    $'cat << EOF\none\ntwo\nEOF\n' \
    $'one\ntwo\n' \
    '' \
    0

# Exit builtin

run_case \
    'exit propagates explicit status' \
    $'exit 7\n' \
    '' \
    '' \
    7

run_case \
    'exit wraps status to unsigned char range' \
    $'exit 300\n' \
    '' \
    '' \
    44

printf '\n======================================\n'
printf 'Total:  %d\n' "$TOTAL"
printf 'Passed: %d\n' "$PASSED"
printf 'Failed: %d\n' "$FAILED"

if [ "$FAILED" -ne 0 ]; then
    exit 1
fi

exit 0
