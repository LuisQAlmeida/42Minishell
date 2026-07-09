/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_wait.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:54:39 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:54:39 by jpedro-g         ###   ########.fr       */
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
		return (128 + WTERMSIG(status));
	return (1);
}

int	exe_pipe_wait(pid_t last_pid)
{
	int		status;
	int		last_status;
	int		signal_status;
	pid_t	pid;

	last_status = 1;
	signal_status = 0;
	sig_set_waiting();
	pid = wait(&status);
	while (pid > 0)
	{
		if (!signal_status && WIFSIGNALED(status)
			&& (WTERMSIG(status) == SIGINT || WTERMSIG(status) == SIGQUIT))
			signal_status = status;
		if (pid == last_pid)
			last_status = status_from_wait(status);
		pid = wait(&status);
	}
	if (signal_status)
		print_signal_msg(signal_status);
	sig_set_interactive();
	return (last_status);
}

int	exe_pipe_heredocs(t_cmd *cmds, t_child_ctx *ctx, int *status)
{
	t_cmd	*cur;

	cur = cmds;
	while (cur)
	{
		if (exe_heredocs_prepare(cur, ctx, status))
			return (1);
		cur = cur->next;
	}
	return (0);
}

int	exe_pipeline(t_cmd *cmds, t_shell *shell, t_token *tokens)
{
	t_child_ctx	ctx;
	int			status;

	status = 0;
	ctx.cmds = cmds;
	ctx.tokens = tokens;
	ctx.shell = shell;
	if (exe_pipe_heredocs(cmds, &ctx, &status))
		return (status);
	return (exe_pipe_chain(cmds, &ctx));
}
