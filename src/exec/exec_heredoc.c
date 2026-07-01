#include "minishell.h"

static int	is_delim(const char *line, const char *delim)
{
	size_t	len;

	len = ft_strlen(delim);
	if (ft_strlen(line) != len)
		return (0);
	return (ft_strncmp(line, delim, len) == 0);
}

static char	*append_char(char *line, char c)
{
	char	buf[2];
	char	*tmp;

	buf[0] = c;
	buf[1] = '\0';
	tmp = ms_strjoin_free(line, buf);
	return (tmp);
}

static char	*read_heredoc_line(void)
{
	char	*line;
	char	c;
	int		ret;

	line = ft_calloc(1, sizeof(char));
	if (!line)
		return (NULL);
	while (g_signal != SIGINT)
	{
		ret = read(STDIN_FILENO, &c, 1);
		if (ret <= 0 || c == '\n')
			break ;
		line = append_char(line, c);
		if (!line)
			return (NULL);
	}
	return (line);
}

static int	read_heredoc_lines(int write_fd, const char *delim)
{
	char	*line;

	while (g_signal != SIGINT)
	{
		write(STDOUT_FILENO, "> ", 2);
		line = read_heredoc_line();
		if (!line)
			break ;
		if (is_delim(line, delim))
		{
			free(line);
			break ;
		}
		write(write_fd, line, ft_strlen(line));
		write(write_fd, "\n", 1);
		free(line);
	}
	return (g_signal == SIGINT);
}

int	setup_heredoc(const char *delim, int *fd, int *status)
{
	int	pipefd[2];

	if (pipe(pipefd) < 0)
	{
		perror("pipe");
		*status = 1;
		return (1);
	}
	setup_heredoc_signals();
	if (read_heredoc_lines(pipefd[1], delim))
	{
		close_redir_fds(pipefd[0], pipefd[1]);
		*fd = -1;
		*status = 130;
		return (1);
	}
	close(pipefd[1]);
	*fd = pipefd[0];
	return (0);
}
