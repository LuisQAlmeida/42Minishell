#include "minishell.h"

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

char	*find_in_path(const char *cmd, char **envp)
{
	char	*path;
	char	**dirs;
	char	*full;
	int		i;

	path = ms_getenv_value("PATH", envp);
	if (!path || path[0] == '\0')
		return (NULL);
	dirs = ft_split(path, ':');
	if (!dirs)
		return (NULL);
	i = 0;
	while (dirs[i])
	{
		full = join_path_cmd(dirs[i], cmd);
		if (!full)
			return (free_argv(dirs), NULL);
		if (access(full, X_OK) == 0)
			return (free_argv(dirs), full);
		free(full);
		i++;
	}
	free_argv(dirs);
	return (NULL);
}

int	ms_is_path(const char *cmd)
{
	if (!cmd)
		return (0);
	return (ft_strchr(cmd, '/') != NULL);
}
