#include "minishell.h"


static int	print_err(const char *msg)
{
	printf("%s\n", msg);
	return (1);
}

int	run_once(const char *line, t_shell *shell)
{
	t_token	*list;
	t_cmd	*cmd;
	t_err	err;

	list = tokenize_line(line, shell, &err);
	if (!list && err == ERR_UNCLOSED_QUOTE)
		return (print_err("minishell: syntax error: unclosed quote"));
	if (!list && err == ERR_MALLOC)
		return (print_err("minishell: error: malloc failed"));
	cmd = parse_simple_cmd(list, &err);
	if (!cmd && err == ERR_MALLOC)
	{
		free_tokens(list);
		return (print_err("minishell: error: malloc failed"));
	}
	shell->last_status = exec_simple_cmd(cmd, shell->envp);
	free_cmd(cmd);
	free_tokens(list);
	return (shell->last_status);
}
