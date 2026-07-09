/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_notfound.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:54:22 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:54:22 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	cmd_is_not_found(t_cmd *cmd, char **envp)
{
	char	*path;

	if (cmd->argc == 0 || blt_is_builtin(cmd->argv[0]))
		return (0);
	if (cmd->argv[0][0] == '\0')
		return (1);
	if (exe_path_direct(cmd->argv[0]))
		return (0);
	path = exe_path_find(cmd->argv[0], envp);
	if (!path)
		return (1);
	return (free(path), 0);
}

int	exe_notfound_muted(t_cmd *cmd, t_child_ctx *ctx)
{
	t_cmd	*cur;

	cur = ctx->cmds;
	while (cur && cur != cmd)
	{
		if (cmd_is_not_found(cur, ctx->shell->envp))
			return (1);
		cur = cur->next;
	}
	return (0);
}
