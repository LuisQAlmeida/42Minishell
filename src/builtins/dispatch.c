#include "minishell.h"

static int	builtin_matches_name(const char *cmd, const char *name)
{
	return (ft_strncmp(cmd, name, ft_strlen(name) + 1) == 0);
}

int	blt_is_builtin(const char *cmd)
{
	if (!cmd)
		return (0);
	return (builtin_matches_name(cmd, "echo")
		|| builtin_matches_name(cmd, "pwd")
		|| builtin_matches_name(cmd, "env")
		|| builtin_matches_name(cmd, "cd")
		|| builtin_matches_name(cmd, "export")
		|| builtin_matches_name(cmd, "unset")
		|| builtin_matches_name(cmd, "exit"));
}

int	blt_execute(t_cmd *cmd, t_shell *shell)
{
	if (builtin_matches_name(cmd->argv[0], "echo"))
		return (blt_echo(cmd));
	if (builtin_matches_name(cmd->argv[0], "pwd"))
		return (blt_pwd());
	if (builtin_matches_name(cmd->argv[0], "env"))
		return (blt_env(shell));
	if (builtin_matches_name(cmd->argv[0], "export"))
		return (blt_export(cmd, shell));
	if (builtin_matches_name(cmd->argv[0], "unset"))
		return (blt_unset(cmd, shell));
	if (builtin_matches_name(cmd->argv[0], "cd"))
		return (blt_cd(cmd, shell));
	if (builtin_matches_name(cmd->argv[0], "exit"))
		return (blt_exit(cmd, shell));
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(cmd->argv[0], STDERR_FILENO);
	ft_putendl_fd(": builtin not implemented yet", STDERR_FILENO);
	return (1);
}
