#include "minishell.h"

char	*ms_strjoin_free(char *s1, char *s2)
{
	size_t	len1;
	size_t	len2;
	char	*res;

	len1 = 0;
	if (s1)
		len1 = ft_strlen(s1);
	len2 = 0;
	if (s2)
		len2 = ft_strlen(s2);
	res = malloc(len1 + len2 + 1);
	if (!res)
	{
		free(s1);
		return (NULL);
	}
	if (len1)
		ft_memcpy(res, s1, len1);
	if (len2)
		ft_memcpy(res + len1, s2, len2);
	res[len1 + len2] = '\0';
	free(s1);
	return (res);
}
