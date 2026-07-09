/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_chain.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:55:09 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:55:09 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	command_chain_has_pipe_error(t_token *tok)
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

static void	command_chain_append(t_cmd **list, t_cmd *new_cmd)
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

static t_token	*command_chain_next_segment(t_token *tok)
{
	while (tok && tok->type != TOK_PIPE)
		tok = tok->next;
	if (tok && tok->type == TOK_PIPE)
		tok = tok->next;
	return (tok);
}

int	grm_count(t_cmd *cmd)
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

t_cmd	*grm_pipeline(t_token *tokens, t_err *err)
{
	t_token	*tok;
	t_cmd	*list;
	t_cmd	*cmd;

	if (command_chain_has_pipe_error(tokens))
	{
		*err = ERR_SYNTAX;
		return (NULL);
	}
	list = NULL;
	tok = tokens;
	while (tok)
	{
		cmd = grm_command(tok, err);
		if (!cmd)
			return (grm_clear(list), NULL);
		command_chain_append(&list, cmd);
		tok = command_chain_next_segment(tok);
	}
	return (list);
}
