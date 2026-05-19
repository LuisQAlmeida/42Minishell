#include "minishell.h"

static int	count_args(t_token *tok)
{
	int	count;

	count = 0;
	while (tok)
	{
		if (tok->type == TOK_WORD && tok->value)
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

static char	**build_argv(t_token *tok, int argc, t_err *err)
{
	char	**argv;
	int		i;

	argv = malloc(sizeof(char *) * (argc + 1));
	if (!argv)
	{
		*err = ERR_MALLOC;
		return (NULL);
	}
	i = 0;
	while (tok)
	{
		if (tok->type == TOK_WORD && tok->value)
		{
			argv[i] = ft_substr(tok->value, 0, ft_strlen(tok->value));
			if (!argv[i])
			{
				*err = ERR_MALLOC;
				free_argv(argv, i);
				return (NULL);
			}
			i++;
		}
		tok = tok->next;
	}
	argv[i] = NULL;
	return (argv);
}

t_cmd	*parse_simple_cmd(t_token *tokens, t_err *err)
{
	t_cmd	*cmd;
	int		argc;

	if (!tokens)
		return (NULL);
	*err = ERR_NONE;
	argc = count_args(tokens);
	if (argc == 0)
		return (NULL);
	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
	{
		*err = ERR_MALLOC;
		return (NULL);
	}
	cmd->argc = argc;
	cmd->argv = build_argv(tokens, argc, err);
	if (!cmd->argv)
	{
		free(cmd);
		return (NULL);
	}
	return (cmd);
}

void	free_cmd(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return ;
	i = 0;
	while (cmd->argv && i < cmd->argc)
	{
		free(cmd->argv[i]);
		i++;
	}
	free(cmd->argv);
	free(cmd);
}