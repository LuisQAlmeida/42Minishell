/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_unset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:55:50 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:55:50 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	**shell_env_without_index(char **envp, int remove_index)
{
	char	**new_env;
	int		count;
	int		i;
	int		j;

	count = sta_env_count(envp);
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

int	sta_env_unset(t_shell *shell, const char *name)
{
	char	**new_env;
	int		index;

	index = sta_env_index(shell->envp, name);
	if (index < 0)
		return (0);
	new_env = shell_env_without_index(shell->envp, index);
	if (!new_env)
		return (1);
	free(shell->envp[index]);
	free(shell->envp);
	shell->envp = new_env;
	return (0);
}
