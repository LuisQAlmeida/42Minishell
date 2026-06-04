#include <readline/readline.h>
#include "minishell.h"

static int	is_blank_line(const char *line)
{
	size_t	i;

	i = 0;
	while (line[i] && ms_isspace(line[i]))
		i++;
	return (line[i] == '\0');
}

int	main_loop(char **envp)
{
	char	*line;
	int		status;

	status = 0;
	setup_interactive_signals();
	while (1)
	{
		line = readline("minishell$ ");
		if (!line)
			break ;
		if (is_blank_line(line))
		{
			free(line);
			continue ;
		}
		status = run_once(line, envp);
		free(line);
	}
	return (status);
}
