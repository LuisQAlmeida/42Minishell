#include "minishell.h"

static char	*append_char(char *line, char c)
{
	char	buf[2];

	buf[0] = c;
	buf[1] = '\0';
	return (ms_strjoin_free(line, buf));
}

static int	read_heredoc_char(char **line, int *eof)
{
	char	c;
	int		ret;

	ret = read(STDIN_FILENO, &c, 1);
	if (ret == 0 && (*line)[0] == '\0')
		return (*eof = 1, 1);
	if (ret == 0)
		return (0);
	if (ret < 0)
		return (-1);
	if (c == '\n')
		return (1);
	*line = append_char(*line, c);
	if (!*line)
		return (-1);
	return (0);
}

static char	*read_heredoc_line(int *eof)
{
	char	*line;
	int		state;

	*eof = 0;
	line = ft_calloc(1, sizeof(char));
	if (!line)
		return (NULL);
	while (g_signal != SIGINT)
	{
		state = read_heredoc_char(&line, eof);
		if (state < 0)
			return (free(line), NULL);
		if (state > 0)
			break ;
	}
	return (line);
}

static int	read_heredoc_lines(int write_fd, const char *delim)
{
	char	*line;
	int		eof;

	while (g_signal != SIGINT)
	{
		ft_putstr_fd("> ", STDOUT_FILENO);
		line = read_heredoc_line(&eof);
		if (!line)
			break ;
		if (eof && line[0] == '\0')
			return (free(line), ft_putchar_fd('\n', STDOUT_FILENO), 0);
		if (ft_strncmp(line, delim, ft_strlen(delim) + 1) == 0)
			return (free(line), 0);
		ft_putendl_fd(line, write_fd);
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
