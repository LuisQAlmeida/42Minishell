#include "../include/minishell.h"
#include <ctype.h>

// para testar:
// cc -Wall -Wextra -Werror src/tokenize/token_utils.c tests/tokenizewords.c  -Iinclude -o tokentest ./tokentest
// correr ./tokentest "escrever varias strings"
// o output vai dizer o type de token (neste caso 0 porque na struct sera o primeiro) e o value dele(neste caso a palavra)
// o output repete para todas as palavras
// essencialmente, neste momento esta capaz de identificar palavras e ignorar spaces e \0
// proximo passo = fazer o tokenizer identificar os restantes types.

static int	ft_strlen(const char *str)
{
	int	i;

	i = 0;
	while (str[i] != '\0')
	{
		i++;
	}
	return (i);
}

static char	*ft_strdup(const char *s)
{
	char	*str;
	int		i;
	int		l;

	i = 0;
	l = ft_strlen(s);
	str = malloc ((l + 1) * sizeof(char));
	if (str == NULL)
		return (NULL);
	while (i < l)
	{
		str[i] = s[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

static char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t			i;
	char			*sub;
	unsigned int	slen;

	slen = ft_strlen(s);
	sub = 0;
	if (start >= slen)
		return (ft_strdup(""));
	if (len > slen + start)
		len = slen + start;
	sub = malloc((len + 1) * sizeof(char));
	if (!sub)
		return (NULL);
	i = 0;
	while (i < len)
	{
		sub[i] = s[start + i];
		i++;
	}
	sub[i] = '\0';
	return (sub);
}

t_token *tokenize_words_only(const char *line)
{
    t_token *head;
    size_t  i;
    size_t  start;
    char    *word;

    head = NULL;
    i = 0;
    
    while (line[i])
    {
        while (isspace(line[i]))
            i++;
        if (line[i] == '\0')
            break ;
        start = i;
        while (!isspace(line[i]) && line[i] != '\0')
            i++;
        word = ft_substr(line, start, i - start);
        token_add_back(&head, token_new(TOK_WORD, word));
    }
    return(head);
}

static void print_tokens(t_token *list)
{
    while (list)
    {
        printf("type=%d value=%s\n", 
            list->type,
            list->value ? list->value : "(null)");
        list = list->next;
    }
}

int main(int ac, char **av)
{
    t_token *list;
    
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
