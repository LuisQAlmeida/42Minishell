#include "minishell.h"

static void	init_pipeline_state(t_pipe_state *state)
{
	state->prev_fd = -1;
	state->pipefd[0] = -1;
	state->pipefd[1] = -1;
	state->last_pid = -1;
}

static int	open_next_pipe(t_cmd *cmd, t_pipe_state *state)
{
	state->pipefd[0] = -1;
	state->pipefd[1] = -1;
	if (!cmd->next)
		return (0);
	if (pipe(state->pipefd) < 0)
	{
		perror("pipe");
		return (1);
	}
	return (0);
}

static void	update_parent_fds(t_cmd *cmd, t_pipe_state *state)
{
	if (state->prev_fd != -1)
		close(state->prev_fd);
	if (cmd->next)
	{
		close(state->pipefd[1]);
		state->prev_fd = state->pipefd[0];
	}
	else
		state->prev_fd = -1;
	state->pipefd[0] = -1;
	state->pipefd[1] = -1;
}

static int	pipeline_fail(t_pipe_state *state)
{
	close_pipeline_fds(state);
	if (state->last_pid != -1)
		wait_pipeline(state->last_pid);
	return (1);
}

int	exec_pipeline_chain(t_cmd *cmds, t_child_ctx *ctx)
{
	t_pipe_state	state;
	pid_t			pid;

	init_pipeline_state(&state);
	while (cmds)
	{
		if (open_next_pipe(cmds, &state))
			return (pipeline_fail(&state));
		pid = fork_pipeline_cmd(cmds, ctx, &state);
		if (pid < 0)
			return (pipeline_fail(&state));
		state.last_pid = pid;
		update_parent_fds(cmds, &state);
		cmds = cmds->next;
	}
	return (wait_pipeline(state.last_pid));
}
