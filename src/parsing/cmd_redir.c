#include "minishell.h"

int	is_redir_type(t_toktype type)
{
	return (type == TOK_REDIR_IN || type == TOK_REDIR_OUT
		|| type == TOK_APPEND || type == TOK_HEREDOC);
}

static t_redir	*redir_new(t_toktype type, char *target, t_err *err)
{
	t_redir	*node;

	node = malloc(sizeof(t_redir));
	if (!node)
	{
		*err = ERR_MALLOC;
		free(target);
		return (NULL);
	}
	node->type = type;
	node->target = target;
	node->fd = -1;
	node->next = NULL;
	return (node);
}

static void	redir_add_back(t_redir **list, t_redir *node)
{
	t_redir	*cur;

	if (!*list)
	{
		*list = node;
		return ;
	}
	cur = *list;
	while (cur->next)
		cur = cur->next;
	cur->next = node;
}

int	add_redir_from_token(t_cmd *cmd, t_token *tok, t_err *err)
{
	char	*target;
	t_redir	*node;

	if (!tok->next || tok->next->type != TOK_WORD)
	{
		*err = ERR_SYNTAX;
		return (0);
	}
	target = ft_substr(tok->next->value, 0, ft_strlen(tok->next->value));
	if (!target)
	{
		*err = ERR_MALLOC;
		return (0);
	}
	node = redir_new(tok->type, target, err);
	if (!node)
		return (0);
	redir_add_back(&cmd->redirs, node);
	return (1);
}
