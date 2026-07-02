#include <readline/readline.h>
#include "minishell.h"

volatile sig_atomic_t	g_signal;

static void	interactive_handler(int sig)
{
	g_signal = sig;
	if (sig == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

static void	heredoc_handler(int sig)
{
	g_signal = sig;
	if (sig == SIGINT)
		write(STDOUT_FILENO, "\n", 1);
}

void	setup_interactive_signals(void)
{
	struct sigaction	sa;

	g_signal = 0;
	sa.sa_handler = interactive_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}

void	setup_heredoc_signals(void)
{
	struct sigaction	sa;

	g_signal = 0;
	sa.sa_handler = heredoc_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}
