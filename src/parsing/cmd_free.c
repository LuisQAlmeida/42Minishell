#include "minishell.h"

void	free_cmd(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return ;
	i = 0;
	while (cmd->argv && i < cmd->argc)
	{
		free(cmd->argv[i]);
		i++;
	}
	free(cmd->argv);
	free_redirs(cmd->redirs);
	free(cmd);
}