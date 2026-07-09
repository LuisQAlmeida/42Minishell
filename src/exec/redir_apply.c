/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_apply.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:54:44 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:54:44 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exe_redir_dup(int fd, int std_fd, int *status)
{
	if (dup2(fd, std_fd) < 0)
	{
		perror("dup2");
		close(fd);
		*status = 1;
		return (1);
	}
	close(fd);
	return (0);
}

void	exe_redir_close(int in_fd, int out_fd)
{
	if (in_fd != -1)
		close(in_fd);
	if (out_fd != -1)
		close(out_fd);
}

static int	apply_all_redirs(t_redir *r, int *in_fd,
	int *out_fd, int *status)
{
	while (r)
	{
		if (exe_redir_one(r, in_fd, out_fd, status))
		{
			exe_redir_close(*in_fd, *out_fd);
			return (1);
		}
		r = r->next;
	}
	return (0);
}

int	exe_redir_apply(t_cmd *cmd, int *status)
{
	int	in_fd;
	int	out_fd;

	in_fd = -1;
	out_fd = -1;
	if (apply_all_redirs(cmd->redirs, &in_fd, &out_fd, status))
		return (1);
	if (in_fd != -1 && exe_redir_dup(in_fd, STDIN_FILENO, status))
		return (1);
	if (out_fd != -1
		&& exe_redir_dup(out_fd, STDOUT_FILENO, status))
		return (1);
	return (0);
}
