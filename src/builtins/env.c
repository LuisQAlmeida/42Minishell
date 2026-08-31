#include "minishell.h"

int	blt_env(t_shell *shell)
{
	int	i;

	i = 0;
	while (shell->envp && shell->envp[i])
	{
		if (ft_strchr(shell->envp[i], '='))
			ft_putendl_fd(shell->envp[i], STDOUT_FILENO);
		i++;
	}
	return (0);
}
