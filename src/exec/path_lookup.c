/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_lookup.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:54:35 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:54:35 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	res = sup_join_free_left(tmp, (char *)cmd);
	return (res);
}

char	*exe_path_find(const char *cmd, char **envp)
{
	char	*path;
	char	**dirs;
	char	*full;
	int		i;

	path = sta_env_value("PATH", envp);
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
			return (sup_free_array(dirs), NULL);
		if (access(full, X_OK) == 0)
			return (sup_free_array(dirs), full);
		free(full);
		i++;
	}
	sup_free_array(dirs);
	return (NULL);
}

int	exe_path_direct(const char *cmd)
{
	if (!cmd)
		return (0);
	return (ft_strchr(cmd, '/') != NULL);
}
