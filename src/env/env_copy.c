#include "minishell.h"

static int	env_count(char **envp)
{
	int	count;

	count = 0;
	while (envp && envp[count])
		count++;
	return (count);
}

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
		copy[i] = ft_substr(envp[i], 0, ft_strlen(envp[i]));
		if (!copy[i])
		{
			free_argv(copy);
			return (NULL);
		}
		i++;
	}
	return (copy);
}
