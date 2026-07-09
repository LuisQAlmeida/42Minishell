/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait_child.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:54:57 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:54:57 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_signal_msg(int status)
{
	int	sig;

	if (!WIFSIGNALED(status))
		return ;
	sig = WTERMSIG(status);
	if (sig == SIGINT)
		write(STDOUT_FILENO, "\n", 1);
	else if (sig == SIGQUIT)
		ft_putendl_fd("Quit (core dumped)", STDERR_FILENO);
}

static int	status_from_wait(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
	{
		print_signal_msg(status);
		return (128 + WTERMSIG(status));
	}
	return (1);
}

int	exe_wait_child(pid_t pid)
{
	int	status;

	sig_set_waiting();
	if (waitpid(pid, &status, 0) < 0)
	{
		sig_set_interactive();
		return (1);
	}
	sig_set_interactive();
	return (status_from_wait(status));
}
