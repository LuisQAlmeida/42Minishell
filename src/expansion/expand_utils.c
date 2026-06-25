#include "minishell.h"

int	ms_is_var_start(char c)
{
	return (ft_isalpha(c) || c == '_');
}

int	ms_is_var_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}

char	*ms_getenv_value(const char *name, char **envp)
{
	size_t	len;
	int		i;

	if (!name)
		return ("");
	len = ft_strlen(name);
	i = 0;
	while (envp && envp[i])
	{
		if (!ft_strncmp(envp[i], name, len) && envp[i][len] == '=')
			return (envp[i] + len + 1);
		i++;
	}
	return ("");
}
