#include "minishell.h"

int	exe_stdio_save(int *stdin_save, int *stdout_save)
{
	*stdin_save = dup(STDIN_FILENO);
	*stdout_save = dup(STDOUT_FILENO);
	if (*stdin_save < 0 || *stdout_save < 0)
	{
		if (*stdin_save != -1)
			close(*stdin_save);
		if (*stdout_save != -1)
			close(*stdout_save);
		perror("dup");
		return (1);
	}
	return (0);
}

int	exe_stdio_restore(int stdin_save, int stdout_save)
{
	int	status;

	status = 0;
	if (dup2(stdin_save, STDIN_FILENO) < 0)
	{
		perror("dup2");
		status = 1;
	}
	if (dup2(stdout_save, STDOUT_FILENO) < 0)
	{
		perror("dup2");
		status = 1;
	}
	close(stdin_save);
	close(stdout_save);
	return (status);
}
