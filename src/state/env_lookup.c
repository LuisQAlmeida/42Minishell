#include "minishell.h"

char	*sta_env_value(const char *name, char **envp)
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

static size_t	env_name_len(const char *entry)
{
	size_t	i;

	i = 0;
	while (entry[i] && entry[i] != '=')
		i++;
	return (i);
}

int	sta_env_count(char **envp)
{
	int	count;

	count = 0;
	while (envp && envp[count])
		count++;
	return (count);
}

int	sta_env_index(char **envp, const char *name)
{
	size_t	name_len;
	size_t	entry_len;
	int		i;

	if (!envp || !name)
		return (-1);
	name_len = env_name_len(name);
	i = 0;
	while (envp[i])
	{
		entry_len = env_name_len(envp[i]);
		if (entry_len == name_len
			&& ft_strncmp(envp[i], name, name_len) == 0)
			return (i);
		i++;
	}
	return (-1);
}
