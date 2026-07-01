#include "minishell.h"

static int	str_eq(const char *s1, const char *s2)
{
	return (ft_strncmp(s1, s2, ft_strlen(s2) + 1) == 0);
}

int	is_builtin(const char *cmd)
{
	if (!cmd)
		return (0);
	return (str_eq(cmd, "echo") || str_eq(cmd, "pwd")
		|| str_eq(cmd, "env") || str_eq(cmd, "cd")
		|| str_eq(cmd, "export") || str_eq(cmd, "unset")
		|| str_eq(cmd, "exit"));
}

int	exec_builtin(t_cmd *cmd, t_shell *shell)
{
	if (ft_strncmp(cmd->argv[0], "echo", 5) == 0)
		return (builtin_echo(cmd));
	if (ft_strncmp(cmd->argv[0], "pwd", 4) == 0)
		return (builtin_pwd());
	if (ft_strncmp(cmd->argv[0], "env", 4) == 0)
		return (builtin_env(shell));
	if (ft_strncmp(cmd->argv[0], "export", 7) == 0)
		return (builtin_export(cmd, shell));
	if (ft_strncmp(cmd->argv[0], "unset", 6) == 0)
		return (builtin_unset(cmd, shell));
	if (ft_strncmp(cmd->argv[0], "cd", 3) == 0)
		return (builtin_cd(cmd, shell));
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(cmd->argv[0], STDERR_FILENO);
	ft_putendl_fd(": builtin not implemented yet", STDERR_FILENO);
	return (1);
}
