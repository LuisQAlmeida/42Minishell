#include "minishell.h"

static int	is_word_end(char c)
{
	return (!c || ms_isspace(c) || c == '|'
		|| c == '<' || c == '>');
}

static char	*read_plain_segment(const char *line, size_t *i,
		t_shell *shell, t_err *err)
{
	size_t	start;
	char	*seg;
	char	*expanded;

	start = *i;
	while (line[*i] && !is_word_end(line[*i])
		&& line[*i] != '\'' && line[*i] != '\"')
		(*i)++;
	seg = ft_substr(line, start, *i - start);
	if (!seg)
	{
		*err = ERR_MALLOC;
		return (NULL);
	}
	expanded = expand_env_vars(seg, shell, err);
	free(seg);
	return (expanded);
}

static char	*read_segment(const char *line, size_t *i,
		t_shell *shell, t_err *err)
{
	if (line[*i] == '\'')
		return (read_single_quoted(line, i, err));
	if (line[*i] == '\"')
		return (read_double_quoted(line, i, shell, err));
	return (read_plain_segment(line, i, shell, err));
}

static char	*join_word_segment(char *word, char *seg, t_err *err)
{
	char	*joined;

	joined = ms_strjoin_free(word, seg);
	if (!joined)
		*err = ERR_MALLOC;
	free(seg);
	return (joined);
}

char	*read_word(const char *line, size_t *i,
		t_shell *shell, t_err *err)
{
	char	*word;
	char	*seg;

	word = NULL;
	while (!is_word_end(line[*i]))
	{
		seg = read_segment(line, i, shell, err);
		if (!seg || *err != ERR_NONE)
			return (word);
		word = join_word_segment(word, seg, err);
		if (!word)
			return (NULL);
	}
	return (word);
}
