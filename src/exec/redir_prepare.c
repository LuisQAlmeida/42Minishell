#include "minishell.h"

static int	open_input(const char *path, int *fd, int *status)
{
	*fd = open(path, O_RDONLY);
	if (*fd < 0)
	{
		perror(path);
		*status = 1;
		return (1);
	}
	return (0);
}

static int	open_output(const char *path, int flags, int *fd, int *status)
{
	*fd = open(path, flags, 0644);
	if (*fd < 0)
	{
		perror(path);
		*status = 1;
		return (1);
	}
	return (0);
}

static int	prepare_one_redir(t_redir *r, t_child_ctx *ctx, int *status)
{
	int	fd;

	fd = -1;
	if (r->type == TOK_REDIR_IN && open_input(r->target, &fd, status))
		return (1);
	else if (r->type == TOK_REDIR_OUT
		&& open_output(r->target, O_WRONLY | O_CREAT | O_TRUNC, &fd, status))
		return (1);
	else if (r->type == TOK_APPEND
		&& open_output(r->target, O_WRONLY | O_CREAT | O_APPEND, &fd, status))
		return (1);
	else if (r->type == TOK_HEREDOC
		&& exe_heredoc_setup(r->target, &fd, ctx, status))
		return (1);
	if (r->fd != -1)
		close(r->fd);
	r->fd = fd;
	return (0);
}

int	exe_redir_prepare(t_cmd *cmd, t_child_ctx *ctx, int *status)
{
	t_redir	*r;
	int		had_heredoc;

	had_heredoc = 0;
	r = cmd->redirs;
	while (r)
	{
		if (r->type == TOK_HEREDOC)
			had_heredoc = 1;
		if (prepare_one_redir(r, ctx, status))
		{
			if (had_heredoc)
				sig_set_interactive();
			return (1);
		}
		r = r->next;
	}
	if (had_heredoc)
		sig_set_interactive();
	return (0);
}

int	exe_heredocs_prepare(t_cmd *cmd, t_child_ctx *ctx, int *status)
{
	t_redir	*r;
	int		fd;

	r = cmd->redirs;
	while (r)
	{
		fd = -1;
		if (r->type == TOK_HEREDOC)
		{
			if (exe_heredoc_setup(r->target, &fd, ctx, status))
			{
				sig_set_interactive();
				return (1);
			}
			if (r->fd != -1)
				close(r->fd);
			r->fd = fd;
		}
		r = r->next;
	}
	sig_set_interactive();
	return (0);
}
