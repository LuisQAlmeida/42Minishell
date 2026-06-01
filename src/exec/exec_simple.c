#include "minishell.h"

static void	print_cmd_error(char *cmd, char *msg)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(cmd, STDERR_FILENO);
	ft_putendl_fd(msg, STDERR_FILENO);
}

static void	exec_child(t_cmd *cmd, char **envp)
{
	char	*path;

	if (ms_is_path(cmd->argv[0]))
	{
		execve(cmd->argv[0], cmd->argv, envp);
		perror("minishell");
		_exit(1);
	}
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

int	exec_simple_cmd(t_cmd *cmd, char **envp)
{
	pid_t	pid;
	int		status;

	if (!cmd || cmd->argc == 0)
		return (0);
	pid = fork();
	if (pid < 0)
		return (1);
	if (pid == 0)
		exec_child(cmd, envp);
	if (waitpid(pid, &status, 0) < 0)
		return (1);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}
