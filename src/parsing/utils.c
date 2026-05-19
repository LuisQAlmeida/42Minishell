#include "minishell.h"
#include <string.h>

int	ft_strlen(const char *str)
{
	int	i;

	i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}

static void	ft_strncpy(char *dest, const char *src, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		dest[i] = src[i];
		i++;
	}
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	slen;
	char	*sub;

	slen = ft_strlen(s);
	if (start >= slen)
	{
		sub = malloc(1);
		if (sub)
			sub[0] = '\0';
		return (sub);
	}
	if (len > slen - start)
		len = slen - start;
	sub = malloc(len + 1);
	if (!sub)
		return (NULL);
	ft_strncpy(sub, s + start, len);
	sub[len] = '\0';
	return (sub);
}

char	*ms_strjoin_free(char *s1, char *s2)
{
	size_t	len1;
	size_t	len2;
	char	*res;

	len1 = s1 ? ft_strlen(s1) : 0;
	len2 = s2 ? ft_strlen(s2) : 0;
	res = malloc(len1 + len2 + 1);
	if (!res)
		return (NULL);
	if (len1)
		memcpy(res, s1, len1);
	if (len2)
		memcpy(res + len1, s2, len2);
	res[len1 + len2] = '\0';
	free(s1);
	return (res);
}
