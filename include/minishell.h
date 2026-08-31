/**
 * @file minishell.h
 * @brief Cross-module interface for the Minishell runtime.
 *
 * Defines the maintained data model and selected cross-module contracts.
 * Internal implementation helpers remain intentionally undocumented.
 */

#ifndef MINISHELL_H
# define MINISHELL_H

/* ************************************************************************** */
/*                                INCLUDES                                    */
/* ************************************************************************** */

# include <stdlib.h>
# include <stdio.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <signal.h>
# include <errno.h>
# include <termios.h>
# include "libft.h"

/* ************************************************************************** */
/*                                  TYPES                                     */
/* ************************************************************************** */

/**
 * @brief Token categories produced by the scanner.
 *
 * Operator tokens preserve the shell syntax required by the grammar and
 * execution stages.
 */
typedef enum e_toktype
{
	TOK_WORD,       /**< Command word or redirection operand. */
	TOK_PIPE,       /**< Pipeline separator (`|`). */
	TOK_REDIR_IN,   /**< Input redirection operator (`<`). */
	TOK_REDIR_OUT,  /**< Truncating output redirection operator (`>`). */
	TOK_HEREDOC,    /**< Here-document operator (`<<`). */
	TOK_APPEND      /**< Appending output redirection operator (`>>`). */
}	t_toktype;

/**
 * @brief Error state propagated through parsing-related operations.
 */
typedef enum e_err
{
	ERR_NONE,           /**< No error has been reported. */
	ERR_MALLOC,         /**< Dynamic allocation failed. */
	ERR_UNCLOSED_QUOTE, /**< Input contains an unterminated quote. */
	ERR_SYNTAX          /**< Input violates the supported shell grammar. */
}	t_err;

/* ************************************************************************** */
/*                               STRUCTURES                                   */
/* ************************************************************************** */

/**
 * @brief Node in the scanner token stream.
 *
 * Token nodes are consumed by the grammar stage and released with
 * scn_token_clear().
 */
typedef struct s_token
{
	t_toktype		type;  /**< Token category. */
	char			*value; /**< Owned token payload when present. */
	struct s_token	*next;  /**< Next token in scanner order. */
}	t_token;

/**
 * @brief Redirection attached to a parsed command.
 *
 * The grammar owns the duplicated target string. A prepared descriptor is
 * owned by the node while fd is non-negative and may later be transferred to
 * execution, which resets fd to -1.
 */
typedef struct s_redir
{
	t_toktype		type;   /**< Redirection operator category. */
	char			*target; /**< Owned redirection target string. */
	int				fd;     /**< Prepared descriptor, or -1 when absent. */
	struct s_redir	*next;   /**< Next redirection for the command. */
}	t_redir;

/**
 * @brief Parsed command node within a pipeline.
 *
 * Each node owns its argument vector and redirection list. Command nodes are
 * chained in pipeline order and released with grm_clear().
 */
typedef struct s_cmd
{
	char			**argv;   /**< Owned NULL-terminated argument vector. */
	int				argc;     /**< Number of command arguments. */
	t_redir			*redirs;  /**< Owned redirection list. */
	struct s_cmd	*next;     /**< Next command in pipeline order. */
}	t_cmd;

/**
 * @brief Mutable state owned by an active shell session.
 *
 * The session owns a mutable copy of the environment for its lifetime.
 * last_status represents the most recently established shell status and is
 * also used by `$?` expansion and by `exit` when no explicit status is given.
 */
typedef struct s_shell
{
	char	**envp;       /**< Owned mutable environment array. */
	int		last_status; /**< Most recently established shell exit status. */
	int		should_exit; /**< Non-zero when the parent loop must terminate. */
}	t_shell;

typedef struct s_tokctx
{
	const char	*line;
	size_t		i;
	t_token		*head;
	t_shell		*shell;
	t_err		*err;
}	t_tokctx;

typedef struct s_child_ctx
{
	t_cmd	*cmds;
	t_token	*tokens;
	t_shell	*shell;
}	t_child_ctx;

typedef struct s_pipe_state
{
	int		prev_fd;
	int		pipefd[2];
	pid_t	last_pid;
}	t_pipe_state;

/* ************************************************************************** */
/*                                SESSION                                     */
/* ************************************************************************** */

/**
 * @brief Run the interactive shell session.
 *
 * Creates the shell-owned environment copy, processes input until EOF or an
 * `exit` request, and releases session resources before returning.
 *
 * @param envp Borrowed process environment received from main().
 * @return Final shell status, or 1 if session initialization fails.
 */
int		ses_loop(char **envp);
int		ses_execute_line(const char *line, t_shell *shell);

/* ************************************************************************** */
/*                                  SCAN                                      */
/* ************************************************************************** */

/**
 * @brief Scan one input line into an owned token list.
 *
 * Words are assembled with the current shell state available for expansion.
 * The error output is reset to ERR_NONE before scanning begins.
 *
 * @param line Borrowed input line to scan.
 * @param shell Borrowed shell state used during word expansion.
 * @param err Output error state.
 * @return Owned token list, or NULL on failure or when no tokens are emitted.
 *
 * The caller owns a successful result and releases it with scn_token_clear().
 * When NULL is returned, err distinguishes scanner failure from an empty
 * result.
 */
t_token	*scn_line(const char *line, t_shell *shell, t_err *err);
char	*scn_word(const char *line, size_t *i,
			t_shell *shell, t_err *err);
char	*scn_quote_single(const char *line, size_t *i, t_err *err);
char	*scn_quote_double(const char *line, size_t *i,
			t_shell *shell, t_err *err);
int		scn_emit_word(t_token **head, char *word, t_err *err);
int		scn_emit_operator(t_token **head, t_toktype type, t_err *err);
t_token	*scn_token_new(t_toktype type, char *value);
void	scn_token_add(t_token **lst, t_token *new_tok);
/**
 * @brief Destroy an owned scanner token list.
 *
 * Releases each token payload and every node in the list.
 *
 * @param lst Token list to destroy; NULL is accepted.
 */
void	scn_token_clear(t_token *lst);

/* ************************************************************************** */
/*                                GRAMMAR                                     */
/* ************************************************************************** */

/**
 * @brief Build the command chain represented by a token stream.
 *
 * Commands are created in pipeline order. Argument strings and redirection
 * targets are copied, so ownership of the input token list remains with the
 * caller.
 *
 * @param tokens Borrowed scanner token list.
 * @param err Output error state for syntax or allocation failures.
 * @return Owned command chain, or NULL when construction fails.
 *
 * A successful result must be released with grm_clear().
 */
t_cmd	*grm_pipeline(t_token *tokens, t_err *err);
t_cmd	*grm_command(t_token *tokens, t_err *err);
int		grm_count(t_cmd *cmd);
int		grm_is_redir(t_toktype type);
int		grm_add_redir(t_cmd *cmd, t_token *tok, t_err *err);
/**
 * @brief Destroy an owned command chain.
 *
 * Releases each command argument vector, redirection list and command node.
 * Prepared redirection descriptors still owned by redirection nodes are also
 * closed during destruction.
 *
 * @param cmd Command chain to destroy; NULL is accepted.
 */
void	grm_clear(t_cmd *cmd);
void	grm_redir_clear(t_redir *list);

/* ************************************************************************** */
/*                                EXPAND                                      */
/* ************************************************************************** */

/**
 * @brief Expand shell variables in a string segment.
 *
 * Supports environment-variable expansion and `$?` using the active shell
 * state.
 *
 * @param str Borrowed source string.
 * @param shell Borrowed shell state used for environment and status lookup.
 * @param err In/out error state; set to ERR_MALLOC on allocation failure.
 * @return Newly allocated expanded string, or NULL on failure.
 *
 * The caller owns the returned string.
 */
char	*exp_variables(const char *str, t_shell *shell, t_err *err);
int		exp_name_start(char c);
int		exp_name_char(char c);
char	*exp_env_value(char *name, t_shell *shell, t_err *err);

/* ************************************************************************** */
/*                                  EXEC                                      */
/* ************************************************************************** */

/**
 * @brief Execute a standalone parsed command.
 *
 * Redirections are prepared before dispatch. Redirection-only commands are
 * applied without launching a program. Standalone builtins execute in the
 * parent shell so persistent state changes survive; external commands execute
 * in a child process.
 *
 * @param cmd Borrowed command node.
 * @param shell Mutable shell session state.
 * @param tokens Borrowed token list for child cleanup context.
 * @return Shell-style execution status.
 */
int		exe_simple(t_cmd *cmd, t_shell *shell, t_token *tokens);
/**
 * @brief Execute a parsed command pipeline.
 *
 * Heredocs are prepared before pipeline children are launched. Pipeline
 * stages execute in child processes, and the resulting status is derived from
 * the final pipeline process.
 *
 * @param cmds Borrowed command chain in pipeline order.
 * @param shell Borrowed shell state inherited by pipeline children.
 * @param tokens Borrowed token list for child cleanup context.
 * @return Pipeline status, or the status produced by heredoc preparation
 * failure.
 */
int		exe_pipeline(t_cmd *cmds, t_shell *shell, t_token *tokens);
void	exe_child(t_cmd *cmd, t_child_ctx *ctx);
int		exe_notfound_muted(t_cmd *cmd, t_child_ctx *ctx);
void	exe_child_exit(t_child_ctx *ctx, int status);
int		exe_wait_child(pid_t pid);
int		exe_pipe_chain(t_cmd *cmds, t_child_ctx *ctx);
pid_t	exe_pipe_fork(t_cmd *cmd, t_child_ctx *ctx,
			t_pipe_state *state);
void	exe_pipe_close(t_pipe_state *state);
int		exe_pipe_wait(pid_t last_pid);
int		exe_pipe_heredocs(t_cmd *cmds, t_child_ctx *ctx, int *status);
int		exe_redir_prepare(t_cmd *cmd, t_child_ctx *ctx, int *status);
int		exe_heredocs_prepare(t_cmd *cmd, t_child_ctx *ctx, int *status);
/**
 * @brief Apply a command's redirections to the current process.
 *
 * Redirections are processed in source order. The last applicable input and
 * output descriptors become the effective standard streams. Prepared
 * descriptors stored in redirection nodes are consumed and reset to -1.
 *
 * @param cmd Borrowed command containing the redirection list.
 * @param status Output status updated when a redirection operation fails.
 * @return 0 on success, 1 on failure.
 *
 * On success, STDIN_FILENO and/or STDOUT_FILENO may have been replaced.
 */
int		exe_redir_apply(t_cmd *cmd, int *status);
int		exe_redir_one(t_redir *r, int *in_fd,
			int *out_fd, int *status);
int		exe_redir_dup(int fd, int std_fd, int *status);
void	exe_redir_close(int in_fd, int out_fd);
int		exe_redir_only(t_cmd *cmd, t_shell *shell);
int		exe_heredoc_setup(const char *delim, int *fd,
			t_child_ctx *ctx, int *status);
int		exe_heredoc_read(int write_fd, const char *delim);
int		exe_heredoc_save_terminal(struct termios *saved);
void	exe_heredoc_restore(struct termios *saved, int has_saved);
char	*exe_path_find(const char *cmd, char **envp);
int		exe_path_direct(const char *cmd);
/**
 * @brief Duplicate the current standard input and output descriptors.
 *
 * On success, ownership of both duplicated descriptors is returned to the
 * caller and is normally transferred to exe_stdio_restore().
 *
 * @param stdin_save Output descriptor duplicating STDIN_FILENO.
 * @param stdout_save Output descriptor duplicating STDOUT_FILENO.
 * @return 0 on success, 1 if either duplication fails.
 */
int		exe_stdio_save(int *stdin_save, int *stdout_save);
/**
 * @brief Restore standard input and output from saved descriptors.
 *
 * Both saved descriptors are closed before the function returns, including
 * when a dup2() operation fails. This function therefore consumes them.
 *
 * @param stdin_save Saved standard-input descriptor.
 * @param stdout_save Saved standard-output descriptor.
 * @return 0 when both streams are restored, otherwise 1.
 */
int		exe_stdio_restore(int stdin_save, int stdout_save);

/* ************************************************************************** */
/*                                BUILTINS                                    */
/* ************************************************************************** */

/**
 * @brief Test whether a command name is a supported shell builtin.
 *
 * @param cmd Borrowed command name; NULL is accepted.
 * @return Non-zero for a supported builtin, otherwise 0.
 */
int		blt_is_builtin(const char *cmd);
/**
 * @brief Dispatch a builtin in the current process.
 *
 * Builtins such as cd, export, unset and exit may modify the supplied shell
 * state.
 *
 * @param cmd Borrowed parsed command.
 * @param shell Mutable shell state available to stateful builtins.
 * @return Status returned by the selected builtin, or 1 when dispatch fails.
 */
int		blt_execute(t_cmd *cmd, t_shell *shell);
/**
 * @brief Execute a standalone builtin in the parent shell process.
 *
 * Parent standard input and output are saved before command redirections are
 * applied and restored afterwards. Executing here allows persistent builtin
 * state changes to survive the command.
 *
 * @param cmd Borrowed parsed builtin command.
 * @param shell Mutable parent-shell state.
 * @return Builtin status, or 1 on stdio save or restore failure.
 */
int		blt_execute_parent(t_cmd *cmd, t_shell *shell);
int		blt_echo(t_cmd *cmd);
int		blt_pwd(void);
int		blt_env(t_shell *shell);
int		blt_export(t_cmd *cmd, t_shell *shell);
int		blt_unset(t_cmd *cmd, t_shell *shell);
int		blt_cd(t_cmd *cmd, t_shell *shell);
int		blt_exit(t_cmd *cmd, t_shell *shell);
int		blt_exit_parse(const char *str, long long *value);
int		blt_valid_identifier(const char *arg, int allow_assignment);
void	blt_identifier_error(const char *name, const char *arg);

/* ************************************************************************** */
/*                                  STATE                                     */
/* ************************************************************************** */

/**
 * @brief Create a deep copy of an environment array.
 *
 * @param envp Borrowed NULL-terminated environment array.
 * @return Newly allocated environment copy, or NULL on allocation failure.
 *
 * The caller owns both the returned array and its duplicated strings.
 */
char	**sta_env_copy(char **envp);
/**
 * @brief Allocate a `KEY=value` environment entry.
 *
 * @param key Borrowed variable name.
 * @param value Borrowed variable value.
 * @return Newly allocated entry, or NULL on allocation failure.
 *
 * The caller owns the returned string.
 */
char	*sta_env_entry(const char *key, const char *value);
int		sta_env_count(char **envp);
int		sta_env_index(char **envp, const char *name);
/**
 * @brief Look up the value portion of an environment entry.
 *
 * @param name Borrowed variable name.
 * @param envp Borrowed NULL-terminated environment array.
 * @return Borrowed pointer to the value, or an empty string when not found.
 *
 * The returned pointer is not owned by the caller and must not be freed.
 */
char	*sta_env_value(const char *name, char **envp);
/**
 * @brief Add or update an entry in the shell-owned environment.
 *
 * Existing entries are replaced when an assignment is supplied. A new entry
 * extends the environment array while preserving ownership of existing
 * strings.
 *
 * @param shell Mutable shell whose environment is updated.
 * @param entry Borrowed environment entry or variable name.
 * @return 0 on success or no-op, 1 on allocation failure.
 */
int		sta_env_set(t_shell *shell, const char *entry);
/**
 * @brief Remove a named entry from the shell-owned environment.
 *
 * The removed string and previous environment array are released. Surviving
 * strings remain owned by the replacement environment array.
 *
 * @param shell Mutable shell whose environment is updated.
 * @param name Borrowed variable name.
 * @return 0 on success or when absent, 1 on allocation failure.
 */
int		sta_env_unset(t_shell *shell, const char *name);

/* ************************************************************************** */
/*                                SIGNALS                                     */
/* ************************************************************************** */

/**
 * @brief Signal number recorded by the active input signal handler.
 *
 * A value of 0 represents no currently recorded handled signal. Interactive
 * and heredoc signal modes reset the value when installed. Session and
 * heredoc control flow inspect the value to react to SIGINT outside the
 * asynchronous handler itself.
 */
extern volatile sig_atomic_t	g_signal;

/**
 * @brief Install signal behaviour for the interactive prompt.
 *
 * Resets g_signal, handles SIGINT through the interactive handler and ignores
 * SIGQUIT. Readline's own signal catching is disabled.
 */
void	sig_set_interactive(void);
/**
 * @brief Install signal behaviour while collecting heredoc input.
 *
 * Resets g_signal, installs the heredoc SIGINT handler and ignores SIGQUIT.
 * The SIGINT handler records the signal and interrupts heredoc input.
 */
void	sig_set_heredoc(void);
/**
 * @brief Install parent-shell signal behaviour while waiting for children.
 *
 * Both SIGINT and SIGQUIT are ignored so foreground child processes handle
 * those signals according to their own policy.
 */
void	sig_set_waiting(void);
/**
 * @brief Restore normal execution signals in a child process.
 *
 * SIGINT and SIGQUIT are both restored to their default dispositions before
 * child command execution.
 */
void	sig_set_child(void);

/* ************************************************************************** */
/*                                SUPPORT                                     */
/* ************************************************************************** */

int		sup_is_space(char c);
/**
 * @brief Destroy an owned NULL-terminated string array.
 *
 * Releases every contained string followed by the array itself.
 *
 * @param str Owned string array to destroy; NULL is accepted.
 */
void	sup_free_array(char **str);
/**
 * @brief Concatenate two strings while consuming the left operand.
 *
 * The first string is freed whether allocation succeeds or fails. The second
 * string remains owned by the caller.
 *
 * @param s1 Owned left string; NULL is treated as empty and is consumed.
 * @param s2 Borrowed right string; NULL is treated as empty.
 * @return Newly allocated concatenation, or NULL on allocation failure.
 *
 * The caller owns a successful return value.
 */
char	*sup_join_free_left(char *s1, char *s2);

#endif
