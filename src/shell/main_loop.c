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

static int	init_shell(t_shell *shell, char **envp)
{
	shell->envp = env_dup(envp);
	if (!shell->envp)
	{
		ft_putendl_fd("minishell: error: malloc failed", STDERR_FILENO);
		return (0);
	}
	shell->last_status = 0;
	setup_interactive_signals();
	return (1);
}

static void	cleanup_shell(t_shell *shell)
{
	rl_clear_history();
	free_argv(shell->envp);
}

int	main_loop(char **envp)
{
	t_shell	shell;
	char	*line;

	if (!init_shell(&shell, envp))
		return (1);
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
	cleanup_shell(&shell);
	return (shell.last_status);
}
