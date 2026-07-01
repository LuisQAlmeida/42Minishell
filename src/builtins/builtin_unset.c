#include "minishell.h"

static int	is_valid_identifier(const char *arg)
{
	int	i;

	if (!arg || (!ft_isalpha(arg[0]) && arg[0] != '_'))
		return (0);
	i = 1;
	while (arg[i])
	{
		if (!ft_isalnum(arg[i]) && arg[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

static void	print_unset_error(const char *arg)
{
	ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
	ft_putstr_fd((char *)arg, STDERR_FILENO);
	ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
}

int	builtin_unset(t_cmd *cmd, t_shell *shell)
{
	int	i;
	int	status;

	status = 0;
	i = 1;
	while (cmd->argv[i])
	{
		if (!is_valid_identifier(cmd->argv[i]))
		{
			print_unset_error(cmd->argv[i]);
			status = 1;
		}
		else if (env_unset_entry(shell, cmd->argv[i]))
			return (1);
		i++;
	}
	return (status);
}
