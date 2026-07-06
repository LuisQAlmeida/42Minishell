#include "minishell.h"

char	**sta_env_copy(char **envp)
{
	char	**copy;
	int		count;
	int		i;

	count = sta_env_count(envp);
	copy = ft_calloc(count + 1, sizeof(char *));
	if (!copy)
		return (NULL);
	i = 0;
	while (i < count)
	{
		copy[i] = ft_strdup(envp[i]);
		if (!copy[i])
			return (sup_free_array(copy), NULL);
		i++;
	}
	return (copy);
}
