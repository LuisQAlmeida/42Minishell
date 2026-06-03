#include "minishell.h"

int	main(int ac, char **av, char **envp)
{
	(void)av;
	if (ac != 1)
	{
		printf("Usage: ./minishell\n");
		return (1);
	}
	return (main_loop(envp));
}
