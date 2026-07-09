/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:54:05 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:54:05 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	exit_status_from_number(long long value)
{
	return ((unsigned char)value);
}

static int	exit_print_numeric_error(char *arg)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd(": numeric argument required", STDERR_FILENO);
	return (2);
}

int	blt_exit(t_cmd *cmd, t_shell *shell)
{
	long long	value;
	int			i;

	if (isatty(STDIN_FILENO))
		ft_putendl_fd("exit", STDERR_FILENO);
	i = 1;
	if (cmd->argv[i] && ft_strncmp(cmd->argv[i], "--", 3) == 0)
		i++;
	if (!cmd->argv[i])
	{
		shell->should_exit = 1;
		return (shell->last_status);
	}
	if (!blt_exit_parse(cmd->argv[i], &value))
	{
		shell->should_exit = 1;
		return (exit_print_numeric_error(cmd->argv[i]));
	}
	if (cmd->argv[i + 1])
	{
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		return (1);
	}
	shell->should_exit = 1;
	return (exit_status_from_number(value));
}
