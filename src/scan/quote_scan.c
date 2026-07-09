/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_scan.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:55:18 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:55:18 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*scn_quote_single(const char *line, size_t *i, t_err *err)
{
	size_t	start;
	char	*seg;

	(*i)++;
	start = *i;
	while (line[*i] && line[*i] != '\'')
		(*i)++;
	if (!line[*i])
	{
		*err = ERR_UNCLOSED_QUOTE;
		return (NULL);
	}
	seg = ft_substr(line, start, *i - start);
	if (!seg)
		*err = ERR_MALLOC;
	else
		(*i)++;
	return (seg);
}

char	*scn_quote_double(const char *line, size_t *i,
			t_shell *shell, t_err *err)
{
	size_t	start;
	char	*seg;
	char	*expanded;

	(*i)++;
	start = *i;
	while (line[*i] && line[*i] != '\"')
		(*i)++;
	if (!line[*i])
	{
		*err = ERR_UNCLOSED_QUOTE;
		return (NULL);
	}
	seg = ft_substr(line, start, *i - start);
	if (!seg)
	{
		*err = ERR_MALLOC;
		return (NULL);
	}
	(*i)++;
	expanded = exp_variables(seg, shell, err);
	free(seg);
	return (expanded);
}
