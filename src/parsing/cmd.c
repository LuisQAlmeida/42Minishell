#include "minishell.h"

static int	count_args(t_token *tok)
{
	int	count;

	count = 0;
	while (tok)
	{
		if (tok->type == TOK_WORD)
			count++;
		tok = tok->next;
	}
	return (count);
}

static void	free_argv(char **argv, int count)
{
	int	i;

	if (!argv)
		return ;
	i = 0;
	while (i < count)
	{
		free(argv[i]);
		i++;
	}
	free(argv);
}

void	free_redirs(t_redir *list)
{
	t_redir	*next;

	while (list)
	{
		next = list->next;
		free(list->target);
		free(list);
		list = next;
	}
}

static int	add_redir(t_cmd *cmd, t_toktype type, char *target, t_err *err)
{
	t_redir	*node;
	t_redir	*cur;

	node = malloc(sizeof(t_redir));
	if (!node)
	{
		*err = ERR_MALLOC;
		free(target);
		return (0);
	}
	node->type = type;
	node->target = target;
	node->next = NULL;
	if (!cmd->redirs)
		cmd->redirs = node;
	else
	{
		cur = cmd->redirs;
		while (cur->next)
			cur = cur->next;
		cur->next = node;
	}
	return (1);
}

static int	fill_argv_and_redirs(t_cmd *cmd, t_token *tok, t_err *err)
{
	char	**argv;
	int		i;

	argv = malloc(sizeof(char *) * (cmd->argc + 1));
	if (!argv)
	{
		*err = ERR_MALLOC;
		return (0);
	}
	i = 0;
	while (tok)
	{
		if (tok->type == TOK_WORD)
		{
			argv[i] = ft_substr(tok->value, 0, ft_strlen(tok->value));
			if (!argv[i])
			{
				*err = ERR_MALLOC;
				free_argv(argv, i);
				return (0);
			}
			i++;
		}
		else if (tok->type == TOK_REDIR_IN || tok->type == TOK_REDIR_OUT
			|| tok->type == TOK_APPEND || tok->type == TOK_HEREDOC)
		{
			if (!tok->next || tok->next->type != TOK_WORD)
			{
				*err = ERR_MALLOC;
				free_argv(argv, i);
				return (0);
			}
			if (!add_redir(cmd, tok->type,
					ft_substr(tok->next->value, 0,
						ft_strlen(tok->next->value)), err))
			{
				free_argv(argv, i);
				return (0);
			}
			tok = tok->next;
		}
		tok = tok->next;
	}
	argv[i] = NULL;
	cmd->argv = argv;
	return (1);
}

t_cmd	*parse_simple_cmd(t_token *tokens, t_err *err)
{
	t_cmd	*cmd;

	if (!tokens)
		return (NULL);
	*err = ERR_NONE;
	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
	{
		*err = ERR_MALLOC;
		return (NULL);
	}
	cmd->argv = NULL;
	cmd->argc = count_args(tokens);
	cmd->redirs = NULL;
	if (cmd->argc == 0)
		return (cmd);
	if (!fill_argv_and_redirs(cmd, tokens, err))
	{
		free_redirs(cmd->redirs);
		free(cmd);
		return (NULL);
	}
	return (cmd);
}