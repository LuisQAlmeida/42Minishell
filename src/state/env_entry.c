#include "minishell.h"

char	*sta_env_entry(const char *key, const char *value)
{
	char	*prefix;
	char	*entry;

	prefix = ft_strjoin(key, "=");
	if (!prefix)
		return (NULL);
	entry = ft_strjoin(prefix, value);
	free(prefix);
	return (entry);
}
