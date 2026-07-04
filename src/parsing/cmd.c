#include "minishell.h"

static int	count_args(t_token *tok)
{
	int	count;

	count = 0;
	while (tok && tok->type != TOK_PIPE)
	{
		if (is_redir_type(tok->type))
		{
			if (tok->next)
				tok = tok->next;
		}
		else if (tok->type == TOK_WORD)
			count++;
		tok = tok->next;
	}
	return (count);
}

static int	add_word_arg(char **argv, int *i, t_token *tok, t_err *err)
{
	argv[*i] = ft_strdup(tok->value);
	if (!argv[*i])
	{
		*err = ERR_MALLOC;
		free_argv(argv);
		return (0);
	}
	(*i)++;
	return (1);
}

static int	fill_argv_and_redirs(t_cmd *cmd, t_token *tok, t_err *err)
{
	int	i;

	cmd->argv = ft_calloc(cmd->argc + 1, sizeof(char *));
	if (!cmd->argv)
	{
		*err = ERR_MALLOC;
		return (0);
	}
	i = 0;
	while (tok && tok->type != TOK_PIPE)
	{
		if (tok->type == TOK_WORD)
		{
			if (!add_word_arg(cmd->argv, &i, tok, err))
				return (0);
		}
		else if (is_redir_type(tok->type))
		{
			if (!add_redir_from_token(cmd, tok, err))
				return (free_argv(cmd->argv), cmd->argv = NULL, 0);
			tok = tok->next;
		}
		tok = tok->next;
	}
	return (1);
}

static t_cmd	*cmd_new(t_token *tokens, t_err *err)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
	{
		*err = ERR_MALLOC;
		return (NULL);
	}
	cmd->argv = NULL;
	cmd->argc = count_args(tokens);
	cmd->redirs = NULL;
	cmd->next = NULL;
	return (cmd);
}

t_cmd	*parse_simple_cmd(t_token *tokens, t_err *err)
{
	t_cmd	*cmd;

	if (!tokens || tokens->type == TOK_PIPE)
	{
		*err = ERR_SYNTAX;
		return (NULL);
	}
	*err = ERR_NONE;
	cmd = cmd_new(tokens, err);
	if (!cmd)
		return (NULL);
	if (!fill_argv_and_redirs(cmd, tokens, err))
	{
		free_redirs(cmd->redirs);
		free(cmd);
		return (NULL);
	}
	return (cmd);
}
