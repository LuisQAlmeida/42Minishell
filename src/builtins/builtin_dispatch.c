#include "minishell.h"

static int	builtin_name_matches(const char *cmd, const char *name)
{
	return (ft_strncmp(cmd, name, ft_strlen(name) + 1) == 0);
}

int	is_builtin(const char *cmd)
{
	if (!cmd)
		return (0);
	return (builtin_name_matches(cmd, "echo")
		|| builtin_name_matches(cmd, "pwd")
		|| builtin_name_matches(cmd, "env")
		|| builtin_name_matches(cmd, "cd")
		|| builtin_name_matches(cmd, "export")
		|| builtin_name_matches(cmd, "unset")
		|| builtin_name_matches(cmd, "exit"));
}

int	exec_builtin(t_cmd *cmd, t_shell *shell)
{
	if (builtin_name_matches(cmd->argv[0], "echo"))
		return (builtin_echo(cmd));
	if (builtin_name_matches(cmd->argv[0], "pwd"))
		return (builtin_pwd());
	if (builtin_name_matches(cmd->argv[0], "env"))
		return (builtin_env(shell));
	if (builtin_name_matches(cmd->argv[0], "export"))
		return (builtin_export(cmd, shell));
	if (builtin_name_matches(cmd->argv[0], "unset"))
		return (builtin_unset(cmd, shell));
	if (builtin_name_matches(cmd->argv[0], "cd"))
		return (builtin_cd(cmd, shell));
	if (builtin_name_matches(cmd->argv[0], "exit"))
		return (builtin_exit(cmd, shell));
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(cmd->argv[0], STDERR_FILENO);
	ft_putendl_fd(": builtin not implemented yet", STDERR_FILENO);
	return (1);
}
