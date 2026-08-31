#include "minishell.h"

int	main(int argc, char **argv, char **envp)
{
	(void)argv;
	if (argc != 1)
		return (ft_putendl_fd("Usage: ./minishell", STDOUT_FILENO), 1);
	return (ses_loop(envp));
}
