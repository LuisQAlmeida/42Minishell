#include "minishell.h"

static int	close_pipe(int pipefd[2], int status)
{
	close(pipefd[0]);
	close(pipefd[1]);
	return (status);
}

static int	setup_pipe_end(int pipefd[2], int is_first)
{
	int	pipe_end;
	int	std_fd;

	pipe_end = 0;
	std_fd = STDIN_FILENO;
	if (is_first)
	{
		pipe_end = 1;
		std_fd = STDOUT_FILENO;
	}
	if (dup2(pipefd[pipe_end], std_fd) < 0)
	{
		perror("dup2");
		return (1);
	}
	return (0);
}

static void	pipeline_child(t_cmd *cmd, t_child_ctx *ctx,
	int pipefd[2], int is_first)
{
	setup_child_signals();
	if (setup_pipe_end(pipefd, is_first))
	{
		close_pipe(pipefd, 0);
		child_exit(ctx, 1);
	}
	close_pipe(pipefd, 0);
	exec_child_cmd(cmd, ctx);
}

static pid_t	fork_pipe_child(t_cmd *cmd, t_child_ctx *ctx,
	int pipefd[2], int is_first)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
		pipeline_child(cmd, ctx, pipefd, is_first);
	return (pid);
}

int	exec_two_cmds(t_cmd *cmds, t_child_ctx *ctx)
{
	int		pipefd[2];
	pid_t	first;
	pid_t	second;

	if (pipe(pipefd) < 0)
	{
		perror("pipe");
		return (1);
	}
	first = fork_pipe_child(cmds, ctx, pipefd, 1);
	if (first < 0)
		return (close_pipe(pipefd, 1));
	second = fork_pipe_child(cmds->next, ctx, pipefd, 0);
	if (second < 0)
	{
		close_pipe(pipefd, 0);
		waitpid(first, NULL, 0);
		return (1);
	}
	close_pipe(pipefd, 0);
	return (wait_pipeline(first, second));
}
