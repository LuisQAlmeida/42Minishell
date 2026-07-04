#include "minishell.h"

static void	print_signal_msg(int status)
{
	int	sig;

	if (!WIFSIGNALED(status))
		return ;
	sig = WTERMSIG(status);
	if (sig == SIGINT)
		write(STDOUT_FILENO, "\n", 1);
	else if (sig == SIGQUIT)
		ft_putendl_fd("Quit (core dumped)", STDERR_FILENO);
}

static int	status_from_wait(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
	{
		print_signal_msg(status);
		return (128 + WTERMSIG(status));
	}
	return (1);
}

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
	return (status_from_wait(status));
}
