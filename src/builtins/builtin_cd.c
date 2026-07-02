#include "minishell.h"

static int	set_pwd_entry(t_shell *shell, char *key, char *value)
{
	char	*entry;

	entry = env_make_entry(key, value);
	if (!entry)
		return (1);
	if (env_set_entry(shell, entry))
	{
		free(entry);
		return (1);
	}
	free(entry);
	return (0);
}

static int	update_pwd_vars(t_shell *shell, char *old_pwd)
{
	char	*new_pwd;

	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
		return (1);
	if (set_pwd_entry(shell, "OLDPWD", old_pwd)
		|| set_pwd_entry(shell, "PWD", new_pwd))
	{
		free(new_pwd);
		return (1);
	}
	free(new_pwd);
	return (0);
}

static char	*get_cd_target(t_cmd *cmd, t_shell *shell)
{
	char	*home;

	if (cmd->argc == 1)
	{
		home = ms_getenv_value("HOME", shell->envp);
		if (!home || home[0] == '\0')
		{
			ft_putendl_fd("minishell: cd: HOME not set", STDERR_FILENO);
			return (NULL);
		}
		return (home);
	}
	return (cmd->argv[1]);
}

static int	change_dir(char *target, char **old_pwd)
{
	*old_pwd = getcwd(NULL, 0);
	if (!*old_pwd)
		return (1);
	if (chdir(target) != 0)
	{
		free(*old_pwd);
		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
		perror(target);
		return (1);
	}
	return (0);
}

int	builtin_cd(t_cmd *cmd, t_shell *shell)
{
	char	*target;
	char	*old_pwd;

	if (cmd->argc > 2)
	{
		ft_putendl_fd("minishell: cd: too many arguments", STDERR_FILENO);
		return (1);
	}
	target = get_cd_target(cmd, shell);
	if (!target)
		return (1);
	if (change_dir(target, &old_pwd))
		return (1);
	if (update_pwd_vars(shell, old_pwd))
	{
		free(old_pwd);
		return (1);
	}
	free(old_pwd);
	return (0);
}
