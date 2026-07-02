#include "minishell.h"

static int	status_from_wait(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

int	wait_pipeline(pid_t first, pid_t second)
{
	int	status;
	int	last_status;

	last_status = 1;
	setup_wait_signals();
	if (waitpid(first, &status, 0) < 0)
		last_status = 1;
	if (waitpid(second, &status, 0) < 0)
		last_status = 1;
	else
		last_status = status_from_wait(status);
	setup_interactive_signals();
	return (last_status);
}

int	prepare_pipeline_redirs(t_cmd *cmds, int *status)
{
	t_cmd	*cur;

	cur = cmds;
	while (cur)
	{
		if (prepare_redirs(cur, status))
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
	if (cmd_count(cmds) != 2)
	{
		ft_putstr_fd("minishell: pipeline execution for N commands ",
			STDERR_FILENO);
		ft_putendl_fd("not implemented yet", STDERR_FILENO);
		return (1);
	}
	ctx.cmds = cmds;
	ctx.tokens = tokens;
	ctx.shell = shell;
	return (exec_two_cmds(cmds, &ctx));
}
