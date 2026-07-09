/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_entry.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:55:43 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:55:43 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*sta_env_entry(const char *key, const char *value)
{
	char	*prefix;
	char	*entry;

	prefix = ft_strjoin(key, "=");
	if (!prefix)
		return (NULL);
	entry = ft_strjoin(prefix, value);
	free(prefix);
	return (entry);
}
