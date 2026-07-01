#include "minishell.h"

void	free_argv(char **argv)
{
	int	i;

	if (!argv)
		return ;
	i = 0;
	while (argv[i])
	{
		free(argv[i]);
		i++;
	}
	free(argv);
}

void	free_redirs(t_redir *list)
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

void	free_cmd(t_cmd *cmd)
{
	if (!cmd)
		return ;
	free_argv(cmd->argv);
	free_redirs(cmd->redirs);
	free(cmd);
}
