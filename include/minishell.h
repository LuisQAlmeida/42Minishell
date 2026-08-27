/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:58:14 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:58:14 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

int		ses_loop(char **envp);
int		ses_execute_line(const char *line, t_shell *shell);

/* ************************************************************************** */
/*                                  SCAN                                      */
/* ************************************************************************** */

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
void	scn_token_clear(t_token *lst);

/* ************************************************************************** */
/*                                GRAMMAR                                     */
/* ************************************************************************** */

t_cmd	*grm_pipeline(t_token *tokens, t_err *err);
t_cmd	*grm_command(t_token *tokens, t_err *err);
int		grm_count(t_cmd *cmd);
int		grm_is_redir(t_toktype type);
int		grm_add_redir(t_cmd *cmd, t_token *tok, t_err *err);
void	grm_clear(t_cmd *cmd);
void	grm_redir_clear(t_redir *list);

/* ************************************************************************** */
/*                                EXPAND                                      */
/* ************************************************************************** */

char	*exp_variables(const char *str, t_shell *shell, t_err *err);
int		exp_name_start(char c);
int		exp_name_char(char c);
char	*exp_env_value(char *name, t_shell *shell, t_err *err);

/* ************************************************************************** */
/*                                  EXEC                                      */
/* ************************************************************************** */

int		exe_simple(t_cmd *cmd, t_shell *shell, t_token *tokens);
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
int		exe_redir_apply(t_cmd *cmd, int *status);
int		exe_redir_one(t_redir *r, int *in_fd,
			int *out_fd, int *status);
int		exe_redir_dup(int fd, int std_fd, int *status);
void	exe_redir_close(int in_fd, int out_fd);
int		exe_redir_only(t_cmd *cmd);
int		exe_heredoc_setup(const char *delim, int *fd,
			t_child_ctx *ctx, int *status);
int		exe_heredoc_read(int write_fd, const char *delim);
int		exe_heredoc_save_terminal(struct termios *saved);
void	exe_heredoc_restore(struct termios *saved, int has_saved);
char	*exe_path_find(const char *cmd, char **envp);
int		exe_path_direct(const char *cmd);
int		exe_stdio_save(int *stdin_save, int *stdout_save);
int		exe_stdio_restore(int stdin_save, int stdout_save);

/* ************************************************************************** */
/*                                BUILTINS                                    */
/* ************************************************************************** */

int		blt_is_builtin(const char *cmd);
int		blt_execute(t_cmd *cmd, t_shell *shell);
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

char	**sta_env_copy(char **envp);
char	*sta_env_entry(const char *key, const char *value);
int		sta_env_count(char **envp);
int		sta_env_index(char **envp, const char *name);
char	*sta_env_value(const char *name, char **envp);
int		sta_env_set(t_shell *shell, const char *entry);
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

void	sig_set_interactive(void);
void	sig_set_heredoc(void);
void	sig_set_waiting(void);
void	sig_set_child(void);

/* ************************************************************************** */
/*                                SUPPORT                                     */
/* ************************************************************************** */

int		sup_is_space(char c);
void	sup_free_array(char **str);
char	*sup_join_free_left(char *s1, char *s2);

#endif
