#include "minishell.h"

void	close_pipeline_fds(t_pipe_state *state)
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
	setup_child_signals();
	if (dup_prev_input(state) || dup_next_output(cmd, state))
	{
		close_pipeline_fds(state);
		child_exit(ctx, 1);
	}
	close_pipeline_fds(state);
	exec_child_cmd(cmd, ctx);
}

pid_t	fork_pipeline_cmd(t_cmd *cmd, t_child_ctx *ctx,
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
