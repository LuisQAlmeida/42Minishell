#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/wait.h>
# include <signal.h>
# include "libft.h"

typedef enum e_toktype
{
	TOK_WORD,
	TOK_PIPE,
	TOK_REDIR_IN,
	TOK_REDIR_OUT,
	TOK_HEREDOC,
	TOK_APPEND
}	t_toktype;

typedef struct s_token
{
	t_toktype		type;
	char			*value;
	struct s_token	*next;
}	t_token;

typedef enum e_err
{
	ERR_NONE,
	ERR_MALLOC,
	ERR_UNCLOSED_QUOTE
}	t_err;

typedef struct s_cmd
{
	char	**argv;
	int		argc;
}	t_cmd;

typedef struct s_shell
{
	char	**envp;
	int		last_status;
}	t_shell;

t_token	*tokenize_line(const char *line, t_shell *shell, t_err *err);
t_token	*token_new(t_toktype type, char *value);
void	token_add_back(t_token **lst, t_token *new_tok);
void	free_tokens(t_token *lst);
int		try_add_token(t_token **head, char *word, t_err *err);
int		add_op_token(t_token **head, t_toktype type, t_err *err);

t_cmd	*parse_simple_cmd(t_token *tokens, t_err *err);
void	free_cmd(t_cmd *cmd);

extern volatile sig_atomic_t	g_signal;

void	setup_interactive_signals(void);
int		main_loop(char **envp);
int		run_once(const char *line, t_shell *shell);

int		exec_simple_cmd(t_cmd *cmd, char **envp);
char	*find_in_path(const char *cmd, char **envp);
int		ms_is_path(const char *cmd);

int		ms_isspace(char c);
char	*ms_strjoin_free(char *s1, char *s2);
char	*read_single_quoted(const char *line, size_t *i, t_err *err);
char	*read_double_quoted(const char *line, size_t *i,
			t_shell *shell, t_err *err);

int		ms_is_var_start(char c);
int		ms_is_var_char(char c);
char	*ms_getenv_value(const char *name, char **envp);
char	*copy_env_value(char *name, t_shell *shell, t_err *err);
char	*expand_env_vars(const char *str, t_shell *shell, t_err *err);

#endif
