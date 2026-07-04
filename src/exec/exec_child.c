#include "minishell.h"

void	child_exit(t_child_ctx *ctx, int status)
{
	if (ctx)
	{
		free_tokens(ctx->tokens);
		free_cmd(ctx->cmds);
		if (ctx->shell)
			free_argv(ctx->shell->envp);
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
	int	status;

	execve(cmd->argv[0], cmd->argv, ctx->shell->envp);
	status = 126;
	if (errno == ENOENT || errno == ENOTDIR)
		status = 127;
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	perror(cmd->argv[0]);
	child_exit(ctx, status);
}

static void	exec_from_path(t_cmd *cmd, t_child_ctx *ctx)
{
	char	*path;

	path = find_in_path(cmd->argv[0], ctx->shell->envp);
	if (!path)
	{
		print_cmd_error(cmd->argv[0], ": command not found");
		child_exit(ctx, 127);
	}
	execve(path, cmd->argv, ctx->shell->envp);
	print_cmd_error(path, ": cannot execute");
	free(path);
	child_exit(ctx, 126);
}

void	exec_child_cmd(t_cmd *cmd, t_child_ctx *ctx)
{
	int	status;

	status = 0;
	if (apply_redirs(cmd, &status))
		child_exit(ctx, status);
	if (cmd->argc == 0)
		child_exit(ctx, 0);
	if (is_builtin(cmd->argv[0]))
		child_exit(ctx, exec_builtin(cmd, ctx->shell));
	if (ms_is_path(cmd->argv[0]))
		exec_path_cmd(cmd, ctx);
	else
		exec_from_path(cmd, ctx);
}
