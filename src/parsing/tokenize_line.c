#include "minishell.h"

static int	add_input_operator(t_tokctx *ctx)
{
	if (ctx->line[ctx->i + 1] == '<')
	{
		ctx->i += 2;
		return (add_op_token(&ctx->head, TOK_HEREDOC, ctx->err));
	}
	ctx->i++;
	return (add_op_token(&ctx->head, TOK_REDIR_IN, ctx->err));
}

static int	add_output_operator(t_tokctx *ctx)
{
	if (ctx->line[ctx->i + 1] == '>')
	{
		ctx->i += 2;
		return (add_op_token(&ctx->head, TOK_APPEND, ctx->err));
	}
	ctx->i++;
	return (add_op_token(&ctx->head, TOK_REDIR_OUT, ctx->err));
}

static int	add_operator_token(t_tokctx *ctx)
{
	if (ctx->line[ctx->i] == '|')
	{
		ctx->i++;
		return (add_op_token(&ctx->head, TOK_PIPE, ctx->err));
	}
	if (ctx->line[ctx->i] == '<')
		return (add_input_operator(ctx));
	if (ctx->line[ctx->i] == '>')
		return (add_output_operator(ctx));
	return (1);
}

static int	handle_word(t_tokctx *ctx)
{
	char	*word;

	word = read_word(ctx->line, &ctx->i, ctx->shell, ctx->err);
	if (!word || *ctx->err != ERR_NONE)
	{
		if (word)
			free(word);
		free_tokens(ctx->head);
		return (0);
	}
	if (!try_add_token(&ctx->head, word, ctx->err))
		return (0);
	return (1);
}

t_token	*tokenize_line(const char *line, t_shell *shell, t_err *err)
{
	t_tokctx	ctx;

	ctx.line = line;
	ctx.i = 0;
	ctx.head = NULL;
	ctx.shell = shell;
	ctx.err = err;
	*err = ERR_NONE;
	while (ctx.line[ctx.i])
	{
		while (ctx.line[ctx.i] && ms_isspace(ctx.line[ctx.i]))
			ctx.i++;
		if (!ctx.line[ctx.i])
			break ;
		if (ctx.line[ctx.i] == '|' || ctx.line[ctx.i] == '<'
			|| ctx.line[ctx.i] == '>')
		{
			if (!add_operator_token(&ctx))
				return (NULL);
		}
		else if (!handle_word(&ctx))
			return (NULL);
	}
	return (ctx.head);
}
