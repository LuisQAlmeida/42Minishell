#include "minishell.h"

static int	exit_status(long long value)
{
	return ((unsigned char)value);
}

static int	print_numeric_error(char *arg)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd(": numeric argument required", STDERR_FILENO);
	return (2);
}

int	builtin_exit(t_cmd *cmd, t_shell *shell)
{
	long long	value;

	ft_putendl_fd("exit", STDERR_FILENO);
	if (cmd->argc == 1)
	{
		shell->should_exit = 1;
		return (shell->last_status);
	}
	if (!parse_exit_number(cmd->argv[1], &value))
	{
		shell->should_exit = 1;
		return (print_numeric_error(cmd->argv[1]));
	}
	if (cmd->argc > 2)
	{
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		return (1);
	}
	shell->should_exit = 1;
	return (exit_status(value));
}
