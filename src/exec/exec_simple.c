#include "minishell.h"

static void	print_cmd_error(const char *cmd, const char *msg)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd((char *)cmd, STDERR_FILENO);
	ft_putendl_fd((char *)msg, STDERR_FILENO);
}

static void	exec_path_cmd(t_cmd *cmd, char **envp)
{
	execve(cmd->argv[0], cmd->argv, envp);
	perror("minishell");
	_exit(1);
}

static void	exec_from_path(t_cmd *cmd, char **envp)
{
	char	*path;

	path = find_in_path(cmd->argv[0], envp);
	if (!path)
	{
		print_cmd_error(cmd->argv[0], ": command not found");
		_exit(127);
	}
	execve(path, cmd->argv, envp);
	print_cmd_error(path, ": cannot execute");
	free(path);
	_exit(126);
}

static void	exec_child(t_cmd *cmd, t_shell *shell)
{
	int	status;

	status = 0;
	if (apply_redirs(cmd, &status))
		_exit(status);
	if (cmd->argc == 0)
		_exit(0);
	if (is_builtin(cmd->argv[0]))
		_exit(exec_builtin(cmd, shell));
	if (ms_is_path(cmd->argv[0]))
		exec_path_cmd(cmd, shell->envp);
	else
		exec_from_path(cmd, shell->envp);
}

int	exec_simple_cmd(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	if (!cmd)
		return (0);
	status = 0;
	if (prepare_redirs(cmd, &status))
		return (status);
	if (cmd->argc == 0)
		return (exec_redir_only(cmd));
	pid = fork();
	if (pid < 0)
		return (1);
	if (pid == 0)
	{
		setup_child_signals();
		exec_child(cmd, shell);
	}
	return (wait_child(pid));
}
