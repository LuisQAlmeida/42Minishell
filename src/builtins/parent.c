#include "minishell.h"

int	blt_execute_parent(t_cmd *cmd, t_shell *shell)
{
	int	stdin_save;
	int	stdout_save;
	int	status;

	status = 0;
	stdin_save = -1;
	stdout_save = -1;
	if (exe_stdio_save(&stdin_save, &stdout_save))
		return (1);
	if (exe_redir_apply(cmd, &status))
	{
		exe_stdio_restore(stdin_save, stdout_save);
		return (status);
	}
	status = blt_execute(cmd, shell);
	if (exe_stdio_restore(stdin_save, stdout_save))
		return (1);
	return (status);
}
