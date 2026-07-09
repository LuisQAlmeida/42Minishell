/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   word_scan.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:55:29 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:55:29 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	word_reached_end(char c)
{
	return (!c || sup_is_space(c) || c == '|'
		|| c == '<' || c == '>');
}

static char	*word_read_plain_segment(const char *line, size_t *i,
		t_shell *shell, t_err *err)
{
	size_t	start;
	char	*seg;
	char	*expanded;

	start = *i;
	while (line[*i] && !word_reached_end(line[*i])
		&& line[*i] != '\'' && line[*i] != '\"')
		(*i)++;
	seg = ft_substr(line, start, *i - start);
	if (!seg)
	{
		*err = ERR_MALLOC;
		return (NULL);
	}
	expanded = exp_variables(seg, shell, err);
	free(seg);
	return (expanded);
}

static char	*word_read_segment(const char *line, size_t *i,
		t_shell *shell, t_err *err)
{
	if (line[*i] == '\'')
		return (scn_quote_single(line, i, err));
	if (line[*i] == '\"')
		return (scn_quote_double(line, i, shell, err));
	return (word_read_plain_segment(line, i, shell, err));
}

static char	*word_append_segment(char *word, char *seg, t_err *err)
{
	char	*joined;

	joined = sup_join_free_left(word, seg);
	if (!joined)
		*err = ERR_MALLOC;
	free(seg);
	return (joined);
}

char	*scn_word(const char *line, size_t *i,
		t_shell *shell, t_err *err)
{
	char	*word;
	char	*seg;

	word = NULL;
	while (!word_reached_end(line[*i]))
	{
		seg = word_read_segment(line, i, shell, err);
		if (!seg || *err != ERR_NONE)
			return (word);
		word = word_append_segment(word, seg, err);
		if (!word)
			return (NULL);
	}
	return (word);
}
