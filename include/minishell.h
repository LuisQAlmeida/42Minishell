#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <ctype.h>
# include <unistd.h>
# include <sys/wait.h>

typedef enum e_toktype
{
	TOK_WORD, // 0
	TOK_PIPE, // 1
	TOK_REDIR_IN, // 2
	TOK_REDIR_OUT, // 3
	TOK_HEREDOC, // 4
	TOK_APPEND // 5
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

t_token	*tokenize_line(const char *line, t_err *err);
t_token	*token_new(t_toktype type, char *value);
void	token_add_back(t_token **lst, t_token *new_tok);
void	free_tokens(t_token *lst);
int		try_add_token(t_token **head, char *word, t_err *err);
int		add_op_token(t_token **head, t_toktype type, t_err *err);

t_cmd	*parse_simple_cmd(t_token *tokens, t_err *err);
void	free_cmd(t_cmd *cmd);

int		exec_simple_cmd(t_cmd *cmd, char **envp);

int		ft_strlen(const char *str);
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*ms_strjoin_free(char *s1, char *s2);

#endif