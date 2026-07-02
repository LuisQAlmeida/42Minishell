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
# include <signal.h>
# include <errno.h>
# include "libft.h"

/* ************************************************************************** */
/*                                  TYPES                                     */
/* ************************************************************************** */

typedef enum e_toktype
{
	TOK_WORD,
	TOK_PIPE,
	TOK_REDIR_IN,
	TOK_REDIR_OUT,
	TOK_HEREDOC,
	TOK_APPEND
}	t_toktype;

typedef enum e_err
{
	ERR_NONE,
	ERR_MALLOC,
	ERR_UNCLOSED_QUOTE,
	ERR_SYNTAX
}	t_err;

/* ************************************************************************** */
/*                                STRUCTURES                                  */
/* ************************************************************************** */

typedef struct s_token
{
	t_toktype		type;
	char			*value;
	struct s_token	*next;
}	t_token;

typedef struct s_redir
{
	t_toktype		type;
	char			*target;
	int				fd;
	struct s_redir	*next;
}	t_redir;

typedef struct s_cmd
{
	char			**argv;
	int				argc;
	t_redir			*redirs;
	struct s_cmd	*next;
}	t_cmd;

typedef struct s_shell
{
	char	**envp;
	int		last_status;
	int		should_exit;
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
/*                            TOKENIZATION                                    */
/* ************************************************************************** */

t_token	*tokenize_line(const char *line, t_shell *shell, t_err *err);
t_token	*token_new(t_toktype type, char *value);
void	token_add_back(t_token **lst, t_token *new_tok);
void	free_tokens(t_token *lst);
int		try_add_token(t_token **head, char *word, t_err *err);
int		add_op_token(t_token **head, t_toktype type, t_err *err);

/* ************************************************************************** */
/*                                PARSING                                     */
/* ************************************************************************** */

t_cmd	*parse_simple_cmd(t_token *tokens, t_err *err);
t_cmd	*parse_pipeline(t_token *tokens, t_err *err);
int		cmd_count(t_cmd *cmd);
void	free_argv(char **argv);
void	free_cmd(t_cmd *cmd);

/* ************************************************************************** */
/*                              REDIRECTIONS                                  */
/* ************************************************************************** */

void	free_redirs(t_redir *list);
int		is_redir_type(t_toktype type);
int		add_redir_from_token(t_cmd *cmd, t_token *tok, t_err *err);

/* ************************************************************************** */
/*                              SHELL LOOP                                    */
/* ************************************************************************** */

int		main_loop(char **envp);
int		run_once(const char *line, t_shell *shell);

/* ************************************************************************** */
/*                                SIGNALS                                     */
/* ************************************************************************** */

extern volatile sig_atomic_t	g_signal;

void	setup_interactive_signals(void);
void	setup_heredoc_signals(void);
void	setup_wait_signals(void);
void	setup_child_signals(void);

/* ************************************************************************** */
/*                               EXECUTION                                    */
/* ************************************************************************** */

int		exec_simple_cmd(t_cmd *cmd, t_shell *shell, t_token *tokens);
int		exec_pipeline(t_cmd *cmds, t_shell *shell, t_token *tokens);
void	exec_child_cmd(t_cmd *cmd, t_child_ctx *ctx);
void	child_exit(t_child_ctx *ctx, int status);
int		exec_pipeline_chain(t_cmd *cmds, t_child_ctx *ctx);
pid_t	fork_pipeline_cmd(t_cmd *cmd, t_child_ctx *ctx,
			t_pipe_state *state);
void	close_pipeline_fds(t_pipe_state *state);
int		wait_pipeline(pid_t last_pid);
int		prepare_pipeline_redirs(t_cmd *cmds, int *status);
int		exec_redir_only(t_cmd *cmd);
int		wait_child(pid_t pid);
int		apply_redirs(t_cmd *cmd, int *status);
int		apply_one_redir(t_redir *r, int *in_fd,
			int *out_fd, int *status);
int		dup_and_close(int fd, int std_fd, int *status);
void	close_redir_fds(int in_fd, int out_fd);
int		prepare_redirs(t_cmd *cmd, int *status);
int		prepare_heredocs(t_cmd *cmd, int *status);
int		setup_heredoc(const char *delim, int *fd, int *status);
char	*find_in_path(const char *cmd, char **envp);
int		ms_is_path(const char *cmd);

/* ************************************************************************** */
/*                                BUILTINS                                    */
/* ************************************************************************** */

int		is_builtin(const char *cmd);
int		parse_exit_number(const char *str, long long *value);
int		exec_builtin(t_cmd *cmd, t_shell *shell);
int		exec_builtin_parent(t_cmd *cmd, t_shell *shell);
int		builtin_echo(t_cmd *cmd);
int		builtin_pwd(void);
int		builtin_env(t_shell *shell);
int		builtin_export(t_cmd *cmd, t_shell *shell);
int		builtin_unset(t_cmd *cmd, t_shell *shell);
int		builtin_cd(t_cmd *cmd, t_shell *shell);
int		builtin_exit(t_cmd *cmd, t_shell *shell);

/* ************************************************************************** */
/*                              ENVIRONMENT                                   */
/* ************************************************************************** */

char	**env_dup(char **envp);
char	*env_make_entry(const char *key, const char *value);
int		env_find_index(char **envp, const char *name);
int		env_set_entry(t_shell *shell, const char *entry);
int		env_unset_entry(t_shell *shell, const char *name);

/* ************************************************************************** */
/*                                EXPANSION                                   */
/* ************************************************************************** */

int		ms_is_var_start(char c);
int		ms_is_var_char(char c);
char	*ms_getenv_value(const char *name, char **envp);
char	*copy_env_value(char *name, t_shell *shell, t_err *err);
char	*expand_env_vars(const char *str, t_shell *shell, t_err *err);

/* ************************************************************************** */
/*                                  UTILS                                     */
/* ************************************************************************** */

int		ms_isspace(char c);
char	*ms_strjoin_free(char *s1, char *s2);
char	*read_word(const char *line, size_t *i,
			t_shell *shell, t_err *err);
char	*read_single_quoted(const char *line, size_t *i, t_err *err);
char	*read_double_quoted(const char *line, size_t *i,
			t_shell *shell, t_err *err);

#endif
