#include "minishell.h"

static int	run_one(const char *line)
{
	t_token	*list;
	t_cmd	*cmd;
	t_err	err;

	list = tokenize_line(line, &err);
	if (!list && err == ERR_UNCLOSED_QUOTE)
		return (0);
	if (!list && err == ERR_MALLOC)
		return (1);
	cmd = parse_simple_cmd(list, &err);
	if (!cmd && err == ERR_MALLOC)
	{
		free_tokens(list);
		return (1);
	}
	free_cmd(cmd);
	free_tokens(list);
	return (0);
}

int	main(void)
{
	const char	*cases[8];
	int			i;
	int			ret;

	cases[0] = "echo hello";
	cases[1] = "echo \"a b c\"";
	cases[2] = "echo 'a b c'";
	cases[3] = "echo \"unterminated";
	cases[4] = "ls -la /tmp";
	cases[5] = "cat < infile";
	cases[6] = "echo hi > outfile";
	cases[7] = "cat << EOF";
	i = 0;
	ret = 0;
	while (i < 8)
	{
		if (run_one(cases[i]) != 0)
			ret = 1;
		i++;
	}
	return (ret);
}
