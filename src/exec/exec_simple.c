#include "minishell.h"

static char	*get_env_value(char **envp, const char *name)
{
	size_t	len;
	int		i;

	len = ft_strlen(name);
	i = 0;
	while (envp && envp[i])
	{
		if (!strncmp(envp[i], name, len) && envp[i][len] == '=')
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
	tmp = malloc(len + 1 + 1);
	if (!tmp)
		return (NULL);
	memcpy(tmp, dir, len);
	tmp[len] = '/';
	tmp[len + 1] = '\0';
	res = ms_strjoin_free(tmp, (char *)cmd);
	return (res);
}

static char	*find_in_path(const char *cmd, char **envp)
{
	char	*path;
	char	*dup;
	char	*token;
	char	*full;

	path = get_env_value(envp, "PATH");
	if (!path)
		return (NULL);
	dup = ft_substr(path, 0, ft_strlen(path));
	if (!dup)
		return (NULL);
	token = strtok(dup, ":");
	while (token)
	{
		full = join_path_cmd(token, cmd);
		if (full && access(full, X_OK) == 0)
		{
			free(dup);
			return (full);
		}
		free(full);
		token = strtok(NULL, ":");
	}
	free(dup);
	return (NULL);
}

static int	is_path(const char *cmd)
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

int	exec_simple_cmd(t_cmd *cmd, char **envp)
{
	pid_t	pid;
	int		status;
	char	*path;

	if (!cmd || cmd->argc == 0)
		return (0);
	pid = fork();
	if (pid < 0)
		return (1);
	if (pid == 0)
	{
		if (is_path(cmd->argv[0]))
			execve(cmd->argv[0], cmd->argv, envp);
		else
		{
			path = find_in_path(cmd->argv[0], envp);
			if (!path)
			{
				fprintf(stderr, "minishell: %s: command not found\n",
					cmd->argv[0]);
				_exit(127);
			}
			execve(path, cmd->argv, envp);
			fprintf(stderr, "minishell: %s: cannot execute\n", path);
			free(path);
			_exit(126);
		}
		perror("minishell");
		_exit(1);
	}
	if (waitpid(pid, &status, 0) < 0)
		return (1);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}