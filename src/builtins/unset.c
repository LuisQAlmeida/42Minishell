#include "minishell.h"

int	blt_unset(t_cmd *cmd, t_shell *shell)
{
	int	i;
	int	status;

	status = 0;
	i = 1;
	while (cmd->argv[i])
	{
		if (!blt_valid_identifier(cmd->argv[i], 0))
		{
			blt_identifier_error("unset", cmd->argv[i]);
			status = 1;
		}
		else if (sta_env_unset(shell, cmd->argv[i]))
			return (1);
		i++;
	}
	return (status);
}
