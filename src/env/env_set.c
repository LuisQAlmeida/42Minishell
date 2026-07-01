#include "minishell.h"

static int	env_count(char **envp)
{
	int	count;

	count = 0;
	while (envp && envp[count])
		count++;
	return (count);
}

static int	has_equal(const char *entry)
{
	int	i;

	i = 0;
	while (entry[i])
	{
		if (entry[i] == '=')
			return (1);
		i++;
	}
	return (0);
}

static int	env_replace(char **envp, int index, const char *entry)
{
	char	*copy;

	copy = ft_substr(entry, 0, ft_strlen(entry));
	if (!copy)
		return (1);
	free(envp[index]);
	envp[index] = copy;
	return (0);
}

static char	**env_new_with_entry(char **envp, const char *entry)
{
	char	**new_env;
	int		count;
	int		i;

	count = env_count(envp);
	new_env = ft_calloc(count + 2, sizeof(char *));
	if (!new_env)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_env[i] = envp[i];
		i++;
	}
	new_env[i] = ft_substr(entry, 0, ft_strlen(entry));
	if (!new_env[i])
	{
		free(new_env);
		return (NULL);
	}
	return (new_env);
}

int	env_set_entry(t_shell *shell, const char *entry)
{
	char	**new_env;
	int		index;

	index = env_find_index(shell->envp, entry);
	if (index >= 0 && !has_equal(entry))
		return (0);
	if (index >= 0)
		return (env_replace(shell->envp, index, entry));
	new_env = env_new_with_entry(shell->envp, entry);
	if (!new_env)
		return (1);
	free(shell->envp);
	shell->envp = new_env;
	return (0);
}
