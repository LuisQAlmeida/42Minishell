#include "minishell.h"

static char	*get_env_value(char **envp, const char *name)
{
	size_t	len;
	int		i;

	len = ft_strlen(name);
	i = 0;
	while (envp && envp[i])
	{
		if (ft_strncmp(envp[i], name, len) == 0 && envp[i][len] == '=')
			return (envp[i] + len + 1);
		i++;
	}
	return (NULL);
}

static char	*join_path_cmd(const char *dir, const char *cmd)
{
	char	*tmp;
	char	*res;
	size_t	len;

	len = ft_strlen(dir);
	tmp = malloc(len + 2);
	if (!tmp)
		return (NULL);
	ft_memcpy(tmp, dir, len);
	tmp[len] = '/';
	tmp[len + 1] = '\0';
	res = ms_strjoin_free(tmp, (char *)cmd);
	return (res);
}

static void	free_split(char **split)
{
	int	i;

	if (!split)
		return ;
	i = 0;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
}

char	*find_in_path(const char *cmd, char **envp)
{
	char	*path;
	char	**dirs;
	char	*full;
	int		i;

	path = get_env_value(envp, "PATH");
	if (!path)
		return (NULL);
	dirs = ft_split(path, ':');
	if (!dirs)
		return (NULL);
	i = 0;
	while (dirs[i])
	{
		full = join_path_cmd(dirs[i], cmd);
		if (!full)
			return (free_split(dirs), NULL);
		if (access(full, X_OK) == 0)
			return (free_split(dirs), full);
		free(full);
		i++;
	}
	free_split(dirs);
	return (NULL);
}

int	ms_is_path(const char *cmd)
{
	int	i;

	i = 0;
	while (cmd[i])
	{
		if (cmd[i] == '/')
			return (1);
		i++;
	}
	return (0);
}
