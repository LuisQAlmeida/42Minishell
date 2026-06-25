#include "minishell.h"

static char	*copy_env_value(char *name, t_shell *shell, t_err *err)
{
	char	*value;
	char	*copy;

	value = ms_getenv_value(name, shell->envp);
	copy = ft_substr(value, 0, ft_strlen(value));
	if (!copy)
		*err = ERR_MALLOC;
	return (copy);
}

static char	*expand_dollar(const char *str, size_t *i,
		t_shell *shell, t_err *err)
{
	size_t	start;
	char	*name;
	char	*part;

	(*i)++;
	if (!ms_is_var_start(str[*i]))
		return (ft_substr("$", 0, 1));
	start = *i;
	while (ms_is_var_char(str[*i]))
		(*i)++;
	name = ft_substr(str, start, *i - start);
	if (!name)
	{
		*err = ERR_MALLOC;
		return (NULL);
	}
	part = copy_env_value(name, shell, err);
	free(name);
	return (part);
}

static char	*next_part(const char *str, size_t *i, t_shell *shell, t_err *err)
{
	char	*part;

	if (str[*i] == '$')
		return (expand_dollar(str, i, shell, err));
	part = ft_substr(str, *i, 1);
	if (!part)
		*err = ERR_MALLOC;
	(*i)++;
	return (part);
}

static int	append_part(char **result, char *part, t_err *err)
{
	char	*tmp;

	tmp = ms_strjoin_free(*result, part);
	if (!tmp)
	{
		*err = ERR_MALLOC;
		return (0);
	}
	*result = tmp;
	return (1);
}

char	*expand_env_vars(const char *str, t_shell *shell, t_err *err)
{
	char	*result;
	char	*part;
	size_t	i;

	result = ft_substr("", 0, 0);
	if (!result)
	{
		*err = ERR_MALLOC;
		return (NULL);
	}
	i = 0;
	while (str[i] && *err == ERR_NONE)
	{
		part = next_part(str, &i, shell, err);
		if (part && *err == ERR_NONE)
			append_part(&result, part, err);
		free(part);
	}
	if (*err != ERR_NONE)
	{
		free(result);
		return (NULL);
	}
	return (result);
}
