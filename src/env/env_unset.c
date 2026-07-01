#include "minishell.h"

static int	env_count(char **envp)
{
	int	count;

	count = 0;
	while (envp && envp[count])
		count++;
	return (count);
}

static char	**env_without_index(char **envp, int remove_index)
{
	char	**new_env;
	int		count;
	int		i;
	int		j;

	count = env_count(envp);
	new_env = ft_calloc(count, sizeof(char *));
	if (!new_env)
		return (NULL);
	i = 0;
	j = 0;
	while (i < count)
	{
		if (i != remove_index)
			new_env[j++] = envp[i];
		i++;
	}
	return (new_env);
}

int	env_unset_entry(t_shell *shell, const char *name)
{
	char	**new_env;
	int		index;

	index = env_find_index(shell->envp, name);
	if (index < 0)
		return (0);
	new_env = env_without_index(shell->envp, index);
	if (!new_env)
		return (1);
	free(shell->envp[index]);
	free(shell->envp);
	shell->envp = new_env;
	return (0);
}
