#include "minishell.h"

static int	print_err(const char *msg)
{
	ft_putendl_fd((char *)msg, STDERR_FILENO);
	return (1);
}

static int	handle_parse_error(t_token *list, t_err err)
{
	free_tokens(list);
	if (err == ERR_MALLOC)
		return (print_err("minishell: error: malloc failed"));
	if (err == ERR_SYNTAX)
		return (print_err("minishell: syntax error near unexpected token"));
	return (1);
}

static int	exec_cmd_list(t_cmd *cmd, t_shell *shell, t_token *tokens)
{
	if (cmd_count(cmd) == 1)
		return (exec_simple_cmd(cmd, shell, tokens));
	return (exec_pipeline(cmd, shell, tokens));
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
	cmd = parse_pipeline(list, &err);
	if (!cmd)
		return (handle_parse_error(list, err));
	shell->last_status = exec_cmd_list(cmd, shell, list);
	free_cmd(cmd);
	free_tokens(list);
	return (shell->last_status);
}
