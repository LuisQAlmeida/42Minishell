#include "minishell.h"

static char	*read_plain_segment(const char *line, size_t *i,
		t_shell *shell, t_err *err)
{
	size_t	start;
	char	*seg;
	char	*expanded;

	start = *i;
	while (line[*i]
		&& !ms_isspace(line[*i])
		&& line[*i] != '\''
		&& line[*i] != '\"'
		&& line[*i] != '|'
		&& line[*i] != '<'
		&& line[*i] != '>')
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

static char	*read_word(const char *line, size_t *i,
		t_shell *shell, t_err *err)
{
	char	*word;
	char	*seg;

	word = NULL;
	while (line[*i]
		&& !ms_isspace(line[*i])
		&& line[*i] != '|'
		&& line[*i] != '<'
		&& line[*i] != '>')
	{
		if (line[*i] == '\'')
			seg = read_single_quoted(line, i, err);
		else if (line[*i] == '\"')
			seg = read_double_quoted(line, i, shell, err);
		else
			seg = read_plain_segment(line, i, shell, err);
		if (!seg || *err != ERR_NONE)
			return (word);
		word = ms_strjoin_free(word, seg);
		if (!word)
		{
			free(seg);
			*err = ERR_MALLOC;
			return (NULL);
		}
		free(seg);
	}
	return (word);
}

static int	add_operator_token(const char *line, size_t *i,
			t_token **head, t_err *err)
{
	if (line[*i] == '|')
	{
		(*i)++;
		return (add_op_token(head, TOK_PIPE, err));
	}
	if (line[*i] == '<')
	{
		if (line[*i + 1] == '<')
		{
			*i += 2;
			return (add_op_token(head, TOK_HEREDOC, err));
		}
		(*i)++;
		return (add_op_token(head, TOK_REDIR_IN, err));
	}
	if (line[*i] == '>')
	{
		if (line[*i + 1] == '>')
		{
			*i += 2;
			return (add_op_token(head, TOK_APPEND, err));
		}
		(*i)++;
		return (add_op_token(head, TOK_REDIR_OUT, err));
	}
	return (1);
}

t_token	*tokenize_line(const char *line, t_shell *shell, t_err *err)
{
	t_token	*head;
	char	*word;
	size_t	i;

	head = NULL;
	*err = ERR_NONE;
	i = 0;
	while (line[i])
	{
		while (line[i] && ms_isspace(line[i]))
			i++;
		if (!line[i])
			break ;
		if (line[i] == '|' || line[i] == '<' || line[i] == '>')
		{
			if (!add_operator_token(line, &i, &head, err))
				return (NULL);
			continue ;
		}
		word = read_word(line, &i, shell, err);
		if (!word || *err != ERR_NONE)
		{
			if (word)
				free(word);
			free_tokens(head);
			return (NULL);
		}
		if (!try_add_token(&head, word, err))
			return (NULL);
	}
	return (head);
}
