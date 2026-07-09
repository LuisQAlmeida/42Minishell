/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_simple.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:54:24 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:54:25 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exe_simple(t_cmd *cmd, t_shell *shell, t_token *tokens)
{
	t_child_ctx	ctx;
	pid_t		pid;
	int			status;

	if (!cmd)
		return (0);
	ctx.cmds = cmd;
	ctx.tokens = tokens;
	ctx.shell = shell;
	status = 0;
	if (exe_redir_prepare(cmd, &ctx, &status))
		return (status);
	if (cmd->argc == 0)
		return (exe_redir_only(cmd));
	if (blt_is_builtin(cmd->argv[0]))
		return (blt_execute_parent(cmd, shell));
	pid = fork();
	if (pid < 0)
		return (1);
	if (pid == 0)
	{
		sig_set_child();
		exe_child(cmd, &ctx);
	}
	return (exe_wait_child(pid));
}
