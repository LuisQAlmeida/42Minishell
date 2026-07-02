#include "minishell.h"

int	wait_child(pid_t pid)
{
	int	status;

	setup_wait_signals();
	if (waitpid(pid, &status, 0) < 0)
	{
		setup_interactive_signals();
		return (1);
	}
	setup_interactive_signals();
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}
