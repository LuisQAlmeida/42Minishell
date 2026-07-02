#include "minishell.h"

int	exec_simple_cmd(t_cmd *cmd, t_shell *shell, t_token *tokens)
{
	t_child_ctx	ctx;
	pid_t		pid;
	int			status;

	if (!cmd)
		return (0);
	status = 0;
	if (prepare_redirs(cmd, &status))
		return (status);
	if (cmd->argc == 0)
		return (exec_redir_only(cmd));
	if (is_builtin(cmd->argv[0]))
		return (exec_builtin_parent(cmd, shell));
	ctx.cmds = cmd;
	ctx.tokens = tokens;
	ctx.shell = shell;
	pid = fork();
	if (pid < 0)
		return (1);
	if (pid == 0)
	{
		setup_child_signals();
		exec_child_cmd(cmd, &ctx);
	}
	return (wait_child(pid));
}
