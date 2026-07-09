/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_read.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:54:27 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 17:42:13 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <readline/readline.h>
#include "minishell.h"

static int	heredoc_is_delimiter(char *line, const char *delim)
{
	size_t	len;

	len = ft_strlen(delim) + 1;
	return (ft_strncmp(line, delim, len) == 0);
}

static void	heredoc_warn_eof(const char *delim)
{
	ft_putstr_fd("minishell: warning: here-document ", STDERR_FILENO);
	ft_putstr_fd("delimited by end-of-file (wanted `", STDERR_FILENO);
	ft_putstr_fd((char *)delim, STDERR_FILENO);
	ft_putendl_fd("')", STDERR_FILENO);
}

static int	heredoc_handle_eof(const char *delim)
{
	if (g_signal == SIGINT)
		return (1);
	heredoc_warn_eof(delim);
	return (0);
}

int	exe_heredoc_read(int write_fd, const char *delim)
{
	char	*line;

	while (g_signal != SIGINT)
	{
		line = readline("> ");
		if (!line)
			return (heredoc_handle_eof(delim));
		if (g_signal == SIGINT)
			return (free(line), 1);
		if (heredoc_is_delimiter(line, delim))
			return (free(line), 0);
		ft_putendl_fd(line, write_fd);
		free(line);
	}
	return (1);
}
