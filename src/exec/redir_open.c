/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_open.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:54:49 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:54:50 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	set_redir_fd(t_redir *r, int *in_fd, int *out_fd, int fd)
{
	if (r->type == TOK_REDIR_IN || r->type == TOK_HEREDOC)
	{
		if (*in_fd != -1)
			close(*in_fd);
		*in_fd = fd;
	}
	else
	{
		if (*out_fd != -1)
			close(*out_fd);
		*out_fd = fd;
	}
}

static int	open_exec_redir(t_redir *r, int *fd, int *status)
{
	if (r->type == TOK_REDIR_IN)
		*fd = open(r->target, O_RDONLY);
	else if (r->type == TOK_REDIR_OUT)
		*fd = open(r->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (r->type == TOK_APPEND)
		*fd = open(r->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		return (0);
	if (*fd < 0)
	{
		perror(r->target);
		*status = 1;
		return (1);
	}
	return (0);
}

int	exe_redir_one(t_redir *r, int *in_fd, int *out_fd, int *status)
{
	int	fd;

	fd = r->fd;
	r->fd = -1;
	if (fd < 0 && r->type != TOK_HEREDOC)
	{
		if (open_exec_redir(r, &fd, status))
			return (1);
	}
	if (fd < 0)
	{
		*status = 1;
		return (1);
	}
	set_redir_fd(r, in_fd, out_fd, fd);
	return (0);
}
