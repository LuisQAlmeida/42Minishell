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

static int	restore_fd(int saved_fd, int std_fd)
{
	int	result;

	result = dup2(saved_fd, std_fd);
	while (result < 0 && errno == EINTR)
		result = dup2(saved_fd, std_fd);
	if (result < 0)
	{
		perror("dup2");
		return (1);
	}
	return (0);
}

int	exe_stdio_restore(int stdin_save, int stdout_save)
{
	int	status;

	status = 0;
	if (restore_fd(stdin_save, STDIN_FILENO))
		status = 1;
	if (restore_fd(stdout_save, STDOUT_FILENO))
		status = 1;
	close(stdin_save);
	close(stdout_save);
	return (status);
}
