#include "minishell.h"

static int	export_print_all(char **envp)
{
	int	i;

	i = 0;
	while (envp && envp[i])
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putendl_fd(envp[i], STDOUT_FILENO);
		i++;
	}
	return (0);
}

int	blt_export(t_cmd *cmd, t_shell *shell)
{
	int	i;
	int	status;

	if (cmd->argc == 1)
		return (export_print_all(shell->envp));
	status = 0;
	i = 1;
	while (cmd->argv[i])
	{
		if (!blt_valid_identifier(cmd->argv[i], 1))
		{
			blt_identifier_error("export", cmd->argv[i]);
			status = 1;
		}
		else if (sta_env_set(shell, cmd->argv[i]))
			return (1);
		i++;
	}
	return (status);
}
