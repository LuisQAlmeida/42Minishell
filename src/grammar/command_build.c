#include "minishell.h"

static int	command_count_args(t_token *tok)
{
	int	count;

	count = 0;
	while (tok && tok->type != TOK_PIPE)
	{
		if (grm_is_redir(tok->type))
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

static int	command_add_arg(char **argv, int *i, t_token *tok, t_err *err)
{
	argv[*i] = ft_strdup(tok->value);
	if (!argv[*i])
	{
		*err = ERR_MALLOC;
		sup_free_array(argv);
		return (0);
	}
	(*i)++;
	return (1);
}

static int	command_fill_parts(t_cmd *cmd, t_token *tok, t_err *err)
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
			if (!command_add_arg(cmd->argv, &i, tok, err))
				return (0);
		}
		else if (grm_is_redir(tok->type))
		{
			if (!grm_add_redir(cmd, tok, err))
				return (sup_free_array(cmd->argv), cmd->argv = NULL, 0);
			tok = tok->next;
		}
		tok = tok->next;
	}
	return (1);
}

static t_cmd	*command_create(t_token *tokens, t_err *err)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
	{
		*err = ERR_MALLOC;
		return (NULL);
	}
	cmd->argv = NULL;
	cmd->argc = command_count_args(tokens);
	cmd->redirs = NULL;
	cmd->next = NULL;
	return (cmd);
}

t_cmd	*grm_command(t_token *tokens, t_err *err)
{
	t_cmd	*cmd;

	if (!tokens || tokens->type == TOK_PIPE)
	{
		*err = ERR_SYNTAX;
		return (NULL);
	}
	*err = ERR_NONE;
	cmd = command_create(tokens, err);
	if (!cmd)
		return (NULL);
	if (!command_fill_parts(cmd, tokens, err))
	{
		grm_redir_clear(cmd->redirs);
		free(cmd);
		return (NULL);
	}
	return (cmd);
}
