#include "libft.h"

size_t	ft_strlen_gnl(const char *s)
{
	size_t	i;

	i = 0;
	if (!s)
		return (0);
	while (s[i] && s[i] != '\n')
		i++;
	if (s[i] == '\n')
		return (i + 1);
	return (i);
}

void	ft_concat(char *nxt_line, char *line, char *buffer, size_t len_buffer)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (line && line[i])
	{
		nxt_line[i] = line[i];
		i++;
	}
	while (j < len_buffer)
	{
		nxt_line[i++] = buffer[j];
		j++;
	}
	nxt_line[i] = '\0';
}

char	*ft_strjoin_gnl(char *line, char *buffer)
{
	size_t	len_line;
	size_t	len_buffer;
	char	*nxt_line;

	len_line = 0;
	while (line && line[len_line])
		len_line++;
	len_buffer = 0;
	while (buffer[len_buffer] && buffer[len_buffer] != '\n')
		len_buffer++;
	if (buffer[len_buffer] == '\n')
		len_buffer++;
	nxt_line = malloc(len_line + len_buffer + 1);
	if (!nxt_line)
		return (free(line), NULL);
	ft_concat(nxt_line, line, buffer, len_buffer);
	free(line);
	return (nxt_line);
}

void	ft_excess(char *buffer)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (buffer[i] && buffer[i] != '\n')
		i++;
	if (buffer[i] == '\n')
		i++;
	while (buffer[i])
		buffer[j++] = buffer[i++];
	while (j < BUFFER_SIZE)
		buffer[j++] = '\0';
}
