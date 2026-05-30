#include "libft.h"

static void	ft_free(char **ptr)
{
	if (ptr && *ptr)
	{
		free(*ptr);
		*ptr = NULL;
	}
}

static char	*ft_build(char *buffer, char *line)
{
	char	*nxt_line;

	nxt_line = ft_strjoin_gnl(line, buffer);
	if (!nxt_line)
		return (NULL);
	ft_excess(buffer);
	return (nxt_line);
}

char	*get_next_line(int fd)
{
	static char	buffer[BUFFER_SIZE + 1];
	char		*nxt_line;
	ssize_t		bytes_read;

	nxt_line = NULL;
	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	while (1)
	{
		if (!buffer[0])
		{
			bytes_read = read(fd, buffer, BUFFER_SIZE);
			if (bytes_read < 0)
				return (ft_free(&nxt_line), NULL);
			if (bytes_read == 0)
				break ;
			buffer[bytes_read] = '\0';
		}
		nxt_line = ft_build(buffer, nxt_line);
		if (!nxt_line || ft_strchr(nxt_line, '\n'))
			break ;
	}
	if (nxt_line && *nxt_line)
		return (nxt_line);
	return (ft_free(&nxt_line), NULL);
}
