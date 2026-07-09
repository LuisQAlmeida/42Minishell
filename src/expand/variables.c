/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   variables.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:55:04 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:55:04 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*expand_status_value(const char *str, size_t *i,
		t_shell *shell, t_err *err)
{
	char	*part;

	if (str[*i] != '?')
		return (NULL);
	(*i)++;
	part = ft_itoa(shell->last_status);
	if (!part)
		*err = ERR_MALLOC;
	return (part);
}

static char	*expand_dollar_value(const char *str, size_t *i,
		t_shell *shell, t_err *err)
{
	size_t	start;
	char	*name;
	char	*part;

	(*i)++;
	part = expand_status_value(str, i, shell, err);
	if (part || *err != ERR_NONE)
		return (part);
	if (!exp_name_start(str[*i]))
		return (ft_strdup("$"));
	start = *i;
	while (exp_name_char(str[*i]))
		(*i)++;
	name = ft_substr(str, start, *i - start);
	if (!name)
	{
		*err = ERR_MALLOC;
		return (NULL);
	}
	part = exp_env_value(name, shell, err);
	return (free(name), part);
}

static char	*expand_next_part(const char *str, size_t *i,
		t_shell *shell, t_err *err)
{
	size_t	start;
	char	*part;

	if (str[*i] == '$')
		return (expand_dollar_value(str, i, shell, err));
	start = *i;
	while (str[*i] && str[*i] != '$')
		(*i)++;
	part = ft_substr(str, start, *i - start);
	if (!part)
		*err = ERR_MALLOC;
	return (part);
}

static int	expand_append_part(char **result, char *part, t_err *err)
{
	char	*tmp;

	tmp = sup_join_free_left(*result, part);
	if (!tmp)
	{
		*err = ERR_MALLOC;
		return (0);
	}
	*result = tmp;
	return (1);
}

char	*exp_variables(const char *str, t_shell *shell, t_err *err)
{
	char	*result;
	char	*part;
	size_t	i;

	result = ft_strdup("");
	if (!result)
	{
		*err = ERR_MALLOC;
		return (NULL);
	}
	i = 0;
	while (str[i] && *err == ERR_NONE)
	{
		part = expand_next_part(str, &i, shell, err);
		if (part && *err == ERR_NONE)
			expand_append_part(&result, part, err);
		free(part);
	}
	if (*err != ERR_NONE)
		return (free(result), NULL);
	return (result);
}
