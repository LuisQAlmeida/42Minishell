#include "minishell.h"
#include <readline/readline.h>

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
static int	is_delim(const char *line, const char *delim)
{
	size_t	len;

	len = ft_strlen(delim);
	if (ft_strlen(line) != len)
		return (0);
	return (ft_strncmp(line, delim, len) == 0);
}

static int	setup_heredoc(const char *delim, int *fd, int *status)
{
	int		pipefd[2];
	char	*line;

	if (pipe(pipefd) < 0)
	{
		perror("pipe");
		*status = 1;
		return (1);
	}
	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (is_delim(line, delim))
		{
			free(line);
			break ;
		}
		write(pipefd[1], line, ft_strlen(line));
		write(pipefd[1], "\n", 1);
		free(line);
	}
	close(pipefd[1]);
	*fd = pipefd[0];
	return (0);
}

static int	apply_redirs(t_cmd *cmd, int *status)
{
	t_redir	*r;
	int		in_fd;
	int		out_fd;

	in_fd = -1;
	out_fd = -1;
	r = cmd->redirs;
	while (r)
	{
		if (r->type == TOK_REDIR_IN)
		{
			if (open_input(r->target, &in_fd, status))
				return (1);
		}
		else if (r->type == TOK_REDIR_OUT)
		{
			if (open_output(r->target,
					O_WRONLY | O_CREAT | O_TRUNC, &out_fd, status))
				return (1);
		}
		else if (r->type == TOK_APPEND)
		{
			if (open_output(r->target,
					O_WRONLY | O_CREAT | O_APPEND, &out_fd, status))
				return (1);
		}
		else if (r->type == TOK_HEREDOC)
		{
			if (setup_heredoc(r->target, &in_fd, status))
				return (1);
		}
		r = r->next;
	}
	if (in_fd != -1)
	{
		if (dup2(in_fd, STDIN_FILENO) < 0)
		{
			perror("dup2");
			*status = 1;
			return (1);
		}
		close(in_fd);
	}
	if (out_fd != -1)
	{
		if (dup2(out_fd, STDOUT_FILENO) < 0)
		{
			perror("dup2");
			*status = 1;
			return (1);
		}
		close(out_fd);
	}
	return (0);
}

int	exec_simple_cmd(t_cmd *cmd, char **envp)
{
	pid_t	pid;
	int		status;
	char	*path;

	if (!cmd || cmd->argc == 0)
		return (0);
	pid = fork();
	if (pid < 0)
		return (1);
	if (pid == 0)
	{
		status = 0;
		if (apply_redirs(cmd, &status))
			_exit(status);
		if (ms_is_path(cmd->argv[0]))
			execve(cmd->argv[0], cmd->argv, envp);
		else
		{
			path = find_in_path(cmd->argv[0], envp);
			if (!path)
			{
				fprintf(stderr, "minishell: %s: command not found\n",
					cmd->argv[0]);
				_exit(127);
			}
			execve(path, cmd->argv, envp);
			fprintf(stderr, "minishell: %s: cannot execute\n", path);
			free(path);
			_exit(126);
		}
		perror("minishell");
		_exit(1);
	}
	if (waitpid(pid, &status, 0) < 0)
		return (1);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}