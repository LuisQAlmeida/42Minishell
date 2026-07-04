#include "minishell.h"

static size_t	env_name_len(const char *entry)
{
	size_t	i;

	i = 0;
	while (entry[i] && entry[i] != '=')
		i++;
	return (i);
}

int	env_count(char **envp)
{
	int	count;

	count = 0;
	while (envp && envp[count])
		count++;
	return (count);
}

int	env_find_index(char **envp, const char *name)
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
