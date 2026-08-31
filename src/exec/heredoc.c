#include "minishell.h"

static int	heredoc_open_pipe(int pipefd[2], int *status)
{
	if (pipe(pipefd) < 0)
	{
		perror("pipe");
		*status = 1;
		return (1);
	}
	return (0);
}

static void	heredoc_child(const char *delim, int pipefd[2], t_child_ctx *ctx)
{
	int	status;
	int	stdin_save;

	stdin_save = dup(STDIN_FILENO);
	close(pipefd[0]);
	sig_set_heredoc();
	status = exe_heredoc_read(pipefd[1], delim);
	if (stdin_save != -1)
	{
		dup2(stdin_save, STDIN_FILENO);
		close(stdin_save);
	}
	close(pipefd[1]);
	if (status)
		exe_child_exit(ctx, 130);
	exe_child_exit(ctx, 0);
}

static int	heredoc_status(int wait_status)
{
	int	sig;

	if (WIFEXITED(wait_status))
		return (WEXITSTATUS(wait_status));
	if (WIFSIGNALED(wait_status))
	{
		sig = WTERMSIG(wait_status);
		if (sig == SIGINT)
			write(STDOUT_FILENO, "\n", 1);
		return (128 + sig);
	}
	return (1);
}

static int	heredoc_parent(pid_t pid, int pipefd[2], int *fd, int *status)
{
	int	wait_status;
	int	code;

	close(pipefd[1]);
	sig_set_waiting();
	if (waitpid(pid, &wait_status, 0) < 0)
	{
		perror("waitpid");
		exe_redir_close(pipefd[0], -1);
		*status = 1;
		return (1);
	}
	code = heredoc_status(wait_status);
	if (code != 0)
	{
		exe_redir_close(pipefd[0], -1);
		*fd = -1;
		*status = code;
		return (1);
	}
	*fd = pipefd[0];
	return (0);
}

int	exe_heredoc_setup(const char *delim, int *fd,
		t_child_ctx *ctx, int *status)
{
	struct termios	saved;
	int				has_saved;
	int				pipefd[2];
	pid_t			pid;
	int				ret;

	if (heredoc_open_pipe(pipefd, status))
		return (1);
	has_saved = exe_heredoc_save_terminal(&saved);
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		exe_heredoc_restore(&saved, has_saved);
		return (exe_redir_close(pipefd[0], pipefd[1]), *status = 1, 1);
	}
	if (pid == 0)
		heredoc_child(delim, pipefd, ctx);
	ret = heredoc_parent(pid, pipefd, fd, status);
	exe_heredoc_restore(&saved, has_saved);
	return (ret);
}
