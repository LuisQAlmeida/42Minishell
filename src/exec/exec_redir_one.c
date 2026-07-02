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

int	apply_one_redir(t_redir *r, int *in_fd, int *out_fd, int *status)
{
	int	fd;

	fd = r->fd;
	r->fd = -1;
	if (fd < 0)
	{
		*status = 1;
		return (1);
	}
	set_redir_fd(r, in_fd, out_fd, fd);
	return (0);
}
