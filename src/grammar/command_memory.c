/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_memory.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:55:11 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:55:12 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	grm_redir_clear(t_redir *list)
{
	t_redir	*next;

	while (list)
	{
		next = list->next;
		if (list->fd != -1)
			close(list->fd);
		free(list->target);
		free(list);
		list = next;
	}
}

void	grm_clear(t_cmd *cmd)
{
	t_cmd	*next;

	while (cmd)
	{
		next = cmd->next;
		sup_free_array(cmd->argv);
		grm_redir_clear(cmd->redirs);
		free(cmd);
		cmd = next;
	}
}
