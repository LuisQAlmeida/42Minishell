#include "minishell.h"

static int	status_from_wait(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

int	wait_pipeline(pid_t last_pid)
{
	int		status;
	int		last_status;
	pid_t	pid;

	last_status = 1;
	setup_wait_signals();
	pid = wait(&status);
	while (pid > 0)
	{
		if (pid == last_pid)
			last_status = status_from_wait(status);
		pid = wait(&status);
	}
	setup_interactive_signals();
	return (last_status);
}

int	prepare_pipeline_redirs(t_cmd *cmds, int *status)
{
	t_cmd	*cur;

	cur = cmds;
	while (cur)
	{
		if (prepare_heredocs(cur, status))
			return (1);
		cur = cur->next;
	}
	return (0);
}

int	exec_pipeline(t_cmd *cmds, t_shell *shell, t_token *tokens)
{
	t_child_ctx	ctx;
	int			status;

	status = 0;
	if (prepare_pipeline_redirs(cmds, &status))
		return (status);
	ctx.cmds = cmds;
	ctx.tokens = tokens;
	ctx.shell = shell;
	return (exec_pipeline_chain(cmds, &ctx));
}
