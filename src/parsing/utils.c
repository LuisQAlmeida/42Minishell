#include "minishell.h"

int	ft_strlen(const char *str)
{
	int	i;

	i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t			i;
	char			*sub;
	unsigned int	slen;

	slen = ft_strlen(s);
	sub = 0;
	if (start >= slen)
	{
		sub = malloc(1);
		if (sub)
			sub[0] = '\0';
		return (sub);
	}
	if (len > slen - start)
		len = slen - start;
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
