#include "minishell.h"

int	dup_and_close(int fd, int std_fd, int *status)
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

void	close_redir_fds(int in_fd, int out_fd)
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
		if (apply_one_redir(r, in_fd, out_fd, status))
		{
			close_redir_fds(*in_fd, *out_fd);
			return (1);
		}
		r = r->next;
	}
	return (0);
}

int	apply_redirs(t_cmd *cmd, int *status)
{
	int	in_fd;
	int	out_fd;

	in_fd = -1;
	out_fd = -1;
	if (apply_all_redirs(cmd->redirs, &in_fd, &out_fd, status))
		return (1);
	if (in_fd != -1 && dup_and_close(in_fd, STDIN_FILENO, status))
		return (1);
	if (out_fd != -1 && dup_and_close(out_fd, STDOUT_FILENO, status))
		return (1);
	return (0);
}
