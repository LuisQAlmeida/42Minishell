#include "minishell.h"

void	grm_redir_clear(t_redir *list)
{
	t_redir	*next;

	while (list)
	{
		next = list->next;
		if (list->fd != -1)
			close(list->fd);
		free(list->target);
		free(list);
		list = next;
	}
}

void	grm_clear(t_cmd *cmd)
{
	t_cmd	*next;

	while (cmd)
	{
		next = cmd->next;
		sup_free_array(cmd->argv);
		grm_redir_clear(cmd->redirs);
		free(cmd);
		cmd = next;
	}
}
