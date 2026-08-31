#include "minishell.h"

char	*exp_env_value(char *name, t_shell *shell, t_err *err)
{
	char	*value;
	char	*copy;

	value = sta_env_value(name, shell->envp);
	copy = ft_strdup(value);
	if (!copy)
		*err = ERR_MALLOC;
	return (copy);
}
