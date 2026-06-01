#include "minishell.h"

static void	print_tokens(t_token *list)
{
	const char	*val;

	while (list)
	{
		val = list->value;
		if (!val)
			val = "(null)";
		printf("type=%d value=%s\n", list->type, val);
		list = list->next;
	}
}

static int	print_err(const char *msg)
{
	printf("%s\n", msg);
	return (1);
}

int	run_once(const char *line, char **envp)
{
	t_token	*list;
	t_cmd	*cmd;
	t_err	err;
	int		status;

	list = tokenize_line(line, &err);
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
	print_tokens(list);
	status = exec_simple_cmd(cmd, envp);
	printf("exit status = %d\n", status);
	free_cmd(cmd);
	free_tokens(list);
	return (status);
}
