/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_command.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:54:22 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:54:22 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exe_child_exit(t_child_ctx *ctx, int status)
{
	if (ctx)
	{
		scn_token_clear(ctx->tokens);
		grm_clear(ctx->cmds);
		if (ctx->shell)
			sup_free_array(ctx->shell->envp);
	}
	exit(status);
}

static void	print_cmd_error(const char *cmd, const char *msg)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd((char *)cmd, STDERR_FILENO);
	ft_putendl_fd((char *)msg, STDERR_FILENO);
}

static void	exec_path_cmd(t_cmd *cmd, t_child_ctx *ctx)
{
	int			status;
	struct stat	st;

	if (stat(cmd->argv[0], &st) == 0 && S_ISDIR(st.st_mode))
	{
		print_cmd_error(cmd->argv[0], ": Is a directory");
		exe_child_exit(ctx, 126);
	}
	execve(cmd->argv[0], cmd->argv, ctx->shell->envp);
	status = 126;
	if (errno == ENOENT || errno == ENOTDIR)
		status = 127;
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	perror(cmd->argv[0]);
	exe_child_exit(ctx, status);
}

static void	exec_from_path(t_cmd *cmd, t_child_ctx *ctx)
{
	char	*path;

	if (cmd->argv[0][0] == '\0')
	{
		if (!exe_notfound_muted(cmd, ctx))
			print_cmd_error(cmd->argv[0], ": command not found");
		exe_child_exit(ctx, 127);
	}
	path = exe_path_find(cmd->argv[0], ctx->shell->envp);
	if (!path)
	{
		if (!exe_notfound_muted(cmd, ctx))
			print_cmd_error(cmd->argv[0], ": command not found");
		exe_child_exit(ctx, 127);
	}
	execve(path, cmd->argv, ctx->shell->envp);
	print_cmd_error(path, ": cannot execute");
	free(path);
	exe_child_exit(ctx, 126);
}

void	exe_child(t_cmd *cmd, t_child_ctx *ctx)
{
	int	status;

	status = 0;
	if (exe_redir_apply(cmd, &status))
		exe_child_exit(ctx, status);
	if (cmd->argc == 0)
		exe_child_exit(ctx, 0);
	if (blt_is_builtin(cmd->argv[0]))
		exe_child_exit(ctx, blt_execute(cmd, ctx->shell));
	if (exe_path_direct(cmd->argv[0]))
		exec_path_cmd(cmd, ctx);
	else
		exec_from_path(cmd, ctx);
}
