#include "minishell.h"

t_token	*tokenize_words_only(const char *line)
{
	t_token	*head;
	size_t	i;
	size_t	start;
	char	*word;

	head = NULL;
	i = 0;
	while (line[i])
	{
		while (ms_isspace(line[i]))
			i++;
		if (line[i] == '\0')
			break ;
		start = i;
		while (!ms_isspace(line[i]) && line[i] != '\0')
			i++;
		word = ft_substr(line, start, i - start);
		token_add_back(&head, token_new(TOK_WORD, word));
	}
	return (head);
}

static void	print_tokens(t_token *list)
{
	while (list)
	{
		printf("type=%d value=%s\n",
			list->type,
			list->value ? list->value : "(null)");
		list = list->next;
	}
}

int	main(int ac, char **av)
{
	t_token	*list;

	if (ac != 2)
	{
		printf("Usage: %s \"command line\"\n", av[0]);
		return (1);
	}
	list = tokenize_words_only(av[1]);
	print_tokens(list);
	free_tokens(list);
	return (0);
}
