#include <readline/readline.h>
#include <readline/history.h>
#include "minishell.h"

static int	ses_line_is_blank(const char *line)
{
	size_t	i;

	i = 0;
	while (line[i] && sup_is_space(line[i]))
		i++;
	return (line[i] == '\0');
}

static int	ses_init(t_shell *shell, char **envp)
{
	shell->envp = sta_env_copy(envp);
	if (!shell->envp)
	{
		ft_putendl_fd("minishell: error: malloc failed", STDERR_FILENO);
		return (0);
	}
	shell->last_status = 0;
	shell->should_exit = 0;
	sig_set_interactive();
	return (1);
}

static void	ses_cleanup(t_shell *shell)
{
	rl_clear_history();
	sup_free_array(shell->envp);
}

static void	ses_handle_signal(t_shell *shell)
{
	if (g_signal == SIGINT)
	{
		shell->last_status = 130;
		g_signal = 0;
	}
}

int	ses_loop(char **envp)
{
	t_shell	shell;
	char	*line;

	if (!ses_init(&shell, envp))
		return (1);
	while (1)
	{
		line = readline("minishell$ ");
		ses_handle_signal(&shell);
		if (!line)
			break ;
		if (ses_line_is_blank(line))
		{
			free(line);
			continue ;
		}
		add_history(line);
		shell.last_status = ses_execute_line(line, &shell);
		free(line);
		if (shell.should_exit)
			break ;
	}
	ses_cleanup(&shell);
	return (shell.last_status);
}
