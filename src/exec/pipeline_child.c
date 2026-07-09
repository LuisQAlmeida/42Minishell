/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_child.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:54:37 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:54:37 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exe_pipe_close(t_pipe_state *state)
{
	if (state->prev_fd != -1)
		close(state->prev_fd);
	if (state->pipefd[0] != -1)
		close(state->pipefd[0]);
	if (state->pipefd[1] != -1)
		close(state->pipefd[1]);
}

static int	dup_prev_input(t_pipe_state *state)
{
	if (state->prev_fd == -1)
		return (0);
	if (dup2(state->prev_fd, STDIN_FILENO) < 0)
	{
		perror("dup2");
		return (1);
	}
	return (0);
}

static int	dup_next_output(t_cmd *cmd, t_pipe_state *state)
{
	if (!cmd->next)
		return (0);
	if (dup2(state->pipefd[1], STDOUT_FILENO) < 0)
	{
		perror("dup2");
		return (1);
	}
	return (0);
}

static void	pipeline_child(t_cmd *cmd, t_child_ctx *ctx,
	t_pipe_state *state)
{
	sig_set_child();
	if (dup_prev_input(state) || dup_next_output(cmd, state))
	{
		exe_pipe_close(state);
		exe_child_exit(ctx, 1);
	}
	exe_pipe_close(state);
	exe_child(cmd, ctx);
}

pid_t	exe_pipe_fork(t_cmd *cmd, t_child_ctx *ctx,
	t_pipe_state *state)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
		pipeline_child(cmd, ctx, state);
	return (pid);
}
