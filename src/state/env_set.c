/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_set.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:55:48 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:55:48 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	env_entry_has_equal(const char *entry)
{
	return (ft_strchr(entry, '=') != NULL);
}

static int	shell_env_replace_at(char **envp, int index, const char *entry)
{
	char	*copy;

	copy = ft_strdup(entry);
	if (!copy)
		return (1);
	free(envp[index]);
	envp[index] = copy;
	return (0);
}

static char	**shell_env_extend(char **envp, const char *entry)
{
	char	**new_env;
	int		count;
	int		i;

	count = sta_env_count(envp);
	new_env = ft_calloc(count + 2, sizeof(char *));
	if (!new_env)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_env[i] = envp[i];
		i++;
	}
	new_env[i] = ft_strdup(entry);
	if (!new_env[i])
		return (free(new_env), NULL);
	return (new_env);
}

int	sta_env_set(t_shell *shell, const char *entry)
{
	char	**new_env;
	int		index;

	index = sta_env_index(shell->envp, entry);
	if (index >= 0 && !env_entry_has_equal(entry))
		return (0);
	if (index >= 0)
		return (shell_env_replace_at(shell->envp, index, entry));
	new_env = shell_env_extend(shell->envp, entry);
	if (!new_env)
		return (1);
	free(shell->envp);
	shell->envp = new_env;
	return (0);
}
