#include "minishell.h"

static int	is_pipe_error(t_token *tok)
{
	if (!tok)
		return (0);
	if (tok->type == TOK_PIPE)
		return (1);
	while (tok)
	{
		if (tok->type == TOK_PIPE)
		{
			if (!tok->next || tok->next->type == TOK_PIPE)
				return (1);
		}
		tok = tok->next;
	}
	return (0);
}

static void	cmd_add_back(t_cmd **list, t_cmd *new_cmd)
{
	t_cmd	*cur;

	if (!*list)
	{
		*list = new_cmd;
		return ;
	}
	cur = *list;
	while (cur->next)
		cur = cur->next;
	cur->next = new_cmd;
}

static t_token	*next_segment(t_token *tok)
{
	while (tok && tok->type != TOK_PIPE)
		tok = tok->next;
	if (tok && tok->type == TOK_PIPE)
		tok = tok->next;
	return (tok);
}

int	cmd_count(t_cmd *cmd)
{
	int	count;

	count = 0;
	while (cmd)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}

t_cmd	*parse_pipeline(t_token *tokens, t_err *err)
{
	t_token	*tok;
	t_cmd	*list;
	t_cmd	*cmd;

	if (is_pipe_error(tokens))
	{
		*err = ERR_SYNTAX;
		return (NULL);
	}
	list = NULL;
	tok = tokens;
	while (tok)
	{
		cmd = parse_simple_cmd(tok, err);
		if (!cmd)
			return (free_cmd(list), NULL);
		cmd_add_back(&list, cmd);
		tok = next_segment(tok);
	}
	return (list);
}
