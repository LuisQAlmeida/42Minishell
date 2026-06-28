#include <readline/readline.h>
#include <readline/history.h>
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
	t_shell	shell;
	char	*line;

	shell.envp = envp;
	shell.last_status = 0;
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
		add_history(line);
		shell.last_status = run_once(line, &shell);
		free(line);
	}
	rl_clear_history();
	return (shell.last_status);
}
