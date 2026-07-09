/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:54:07 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:54:08 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	export_print_entry(const char *entry)
{
	char	*eq;

	ft_putstr_fd("declare -x ", STDOUT_FILENO);
	eq = ft_strchr(entry, '=');
	if (!eq)
	{
		ft_putendl_fd((char *)entry, STDOUT_FILENO);
		return ;
	}
	write(STDOUT_FILENO, entry, eq - entry + 1);
	ft_putstr_fd("\"", STDOUT_FILENO);
	ft_putstr_fd(eq + 1, STDOUT_FILENO);
	ft_putendl_fd("\"", STDOUT_FILENO);
}

static int	export_print_all(char **envp)
{
	int	i;

	i = 0;
	while (envp && envp[i])
		export_print_entry(envp[i++]);
	return (0);
}

static char	*export_append_entry(t_shell *shell, const char *arg, char *eq)
{
	char	*name;
	char	*old;
	char	*entry;

	name = ft_substr(arg, 0, eq - arg - 1);
	if (!name)
		return (NULL);
	old = sta_env_value(name, shell->envp);
	entry = sup_join_free_left(name, "=");
	if (entry)
		entry = sup_join_free_left(entry, old);
	if (entry)
		entry = sup_join_free_left(entry, eq + 1);
	return (entry);
}

static int	export_apply(t_shell *shell, const char *arg)
{
	char	*eq;
	char	*entry;
	int		status;

	eq = ft_strchr(arg, '=');
	if (!eq || eq == arg || *(eq - 1) != '+')
		return (sta_env_set(shell, arg));
	entry = export_append_entry(shell, arg, eq);
	if (!entry)
		return (1);
	status = sta_env_set(shell, entry);
	free(entry);
	return (status);
}

int	blt_export(t_cmd *cmd, t_shell *shell)
{
	int	i;
	int	status;

	if (cmd->argc == 1)
		return (export_print_all(shell->envp));
	status = 0;
	i = 1;
	while (cmd->argv[i])
	{
		if (!blt_valid_identifier(cmd->argv[i], 1))
		{
			blt_identifier_error("export", cmd->argv[i]);
			status = 1;
		}
		else if (export_apply(shell, cmd->argv[i]))
			return (1);
		i++;
	}
	return (status);
}
