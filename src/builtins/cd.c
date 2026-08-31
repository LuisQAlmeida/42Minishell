#include "minishell.h"

static int	cd_set_pwd_entry(t_shell *shell, char *key, char *value)
{
	char	*entry;

	entry = sta_env_entry(key, value);
	if (!entry)
		return (1);
	if (sta_env_set(shell, entry))
		return (free(entry), 1);
	return (free(entry), 0);
}

static int	cd_update_pwd_vars(t_shell *shell, char *old_pwd, int print_pwd)
{
	char	*new_pwd;

	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
		return (1);
	if (cd_set_pwd_entry(shell, "OLDPWD", old_pwd)
		|| cd_set_pwd_entry(shell, "PWD", new_pwd))
		return (free(new_pwd), 1);
	if (print_pwd)
		ft_putendl_fd(new_pwd, STDOUT_FILENO);
	return (free(new_pwd), 0);
}

static char	*cd_get_target(t_cmd *cmd, t_shell *shell, int *print_pwd)
{
	char	*target;

	*print_pwd = 0;
	if (cmd->argc == 1)
	{
		target = sta_env_value("HOME", shell->envp);
		if (!target || target[0] == '\0')
		{
			ft_putendl_fd("minishell: cd: HOME not set", STDERR_FILENO);
			return (NULL);
		}
		return (target);
	}
	if (ft_strncmp(cmd->argv[1], "-", 2) == 0)
	{
		target = sta_env_value("OLDPWD", shell->envp);
		if (!target || target[0] == '\0')
		{
			ft_putendl_fd("minishell: cd: OLDPWD not set", STDERR_FILENO);
			return (NULL);
		}
		*print_pwd = 1;
		return (target);
	}
	return (cmd->argv[1]);
}

static int	cd_change_dir(char *target, char **old_pwd)
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

int	blt_cd(t_cmd *cmd, t_shell *shell)
{
	char	*target;
	char	*old_pwd;
	int		print_pwd;

	if (cmd->argc > 2)
	{
		ft_putendl_fd("minishell: cd: too many arguments", STDERR_FILENO);
		return (1);
	}
	target = cd_get_target(cmd, shell, &print_pwd);
	if (!target)
		return (1);
	if (cd_change_dir(target, &old_pwd))
		return (1);
	if (cd_update_pwd_vars(shell, old_pwd, print_pwd))
		return (free(old_pwd), 1);
	return (free(old_pwd), 0);
}
