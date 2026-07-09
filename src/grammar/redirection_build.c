/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_build.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:55:14 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:55:14 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	grm_is_redir(t_toktype type)
{
	return (type == TOK_REDIR_IN || type == TOK_REDIR_OUT
		|| type == TOK_APPEND || type == TOK_HEREDOC);
}

static t_redir	*redirection_create(t_toktype type, char *target, t_err *err)
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

static void	redirection_append(t_redir **list, t_redir *node)
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

int	grm_add_redir(t_cmd *cmd, t_token *tok, t_err *err)
{
	char	*target;
	t_redir	*node;

	if (!tok->next || tok->next->type != TOK_WORD)
	{
		*err = ERR_SYNTAX;
		return (0);
	}
	target = ft_strdup(tok->next->value);
	if (!target)
	{
		*err = ERR_MALLOC;
		return (0);
	}
	node = redirection_create(tok->type, target, err);
	if (!node)
		return (0);
	redirection_append(&cmd->redirs, node);
	return (1);
}
