#include "minishell.h"

volatile sig_atomic_t	g_signal;

static void	signal_handler(int sig)
{
	g_signal = sig;
}

void	setup_interactive_signals(void)
{
	struct sigaction	sa;

	g_signal = 0;
	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}
