#include "minishell.h"

char	**env_dup(char **envp)
{
	char	**copy;
	int		count;
	int		i;

	count = env_count(envp);
	copy = ft_calloc(count + 1, sizeof(char *));
	if (!copy)
		return (NULL);
	i = 0;
	while (i < count)
	{
		copy[i] = ft_strdup(envp[i]);
		if (!copy[i])
			return (free_argv(copy), NULL);
		i++;
	}
	return (copy);
}
