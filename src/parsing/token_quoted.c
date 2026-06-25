#include "minishell.h"

char	*read_single_quoted(const char *line, size_t *i, t_err *err)
{
	size_t	start;
	char	*seg;

	(*i)++;
	start = *i;
	while (line[*i] && line[*i] != '\'')
		(*i)++;
	if (!line[*i])
	{
		*err = ERR_UNCLOSED_QUOTE;
		return (NULL);
	}
	seg = ft_substr(line, start, *i - start);
	if (!seg)
		*err = ERR_MALLOC;
	else
		(*i)++;
	return (seg);
}

char	*read_double_quoted(const char *line, size_t *i,
		t_shell *shell, t_err *err)
{
	size_t	start;
	char	*seg;
	char	*expanded;

	(*i)++;
	start = *i;
	while (line[*i] && line[*i] != '\"')
		(*i)++;
	if (!line[*i])
	{
		*err = ERR_UNCLOSED_QUOTE;
		return (NULL);
	}
	seg = ft_substr(line, start, *i - start);
	if (!seg)
	{
		*err = ERR_MALLOC;
		return (NULL);
	}
	(*i)++;
	expanded = expand_env_vars(seg, shell, err);
	free(seg);
	return (expanded);
}
