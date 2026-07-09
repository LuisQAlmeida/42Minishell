/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   line_scan.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:55:16 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:55:16 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	scan_input_operator(t_tokctx *ctx)
{
	if (ctx->line[ctx->i + 1] == '<')
	{
		ctx->i += 2;
		return (scn_emit_operator(&ctx->head, TOK_HEREDOC, ctx->err));
	}
	ctx->i++;
	return (scn_emit_operator(&ctx->head, TOK_REDIR_IN, ctx->err));
}

static int	scan_output_operator(t_tokctx *ctx)
{
	if (ctx->line[ctx->i + 1] == '>')
	{
		ctx->i += 2;
		return (scn_emit_operator(&ctx->head, TOK_APPEND, ctx->err));
	}
	ctx->i++;
	return (scn_emit_operator(&ctx->head, TOK_REDIR_OUT, ctx->err));
}

static int	scan_operator_token(t_tokctx *ctx)
{
	if (ctx->line[ctx->i] == '|')
	{
		ctx->i++;
		return (scn_emit_operator(&ctx->head, TOK_PIPE, ctx->err));
	}
	if (ctx->line[ctx->i] == '<')
		return (scan_input_operator(ctx));
	if (ctx->line[ctx->i] == '>')
		return (scan_output_operator(ctx));
	return (1);
}

static int	scan_word_token(t_tokctx *ctx)
{
	char	*word;
	size_t	start;

	start = ctx->i;
	word = scn_word(ctx->line, &ctx->i, ctx->shell, ctx->err);
	if (!word || *ctx->err != ERR_NONE)
	{
		if (word)
			free(word);
		scn_token_clear(ctx->head);
		return (0);
	}
	while (start < ctx->i && ctx->line[start] != '\''
		&& ctx->line[start] != '\"')
		start++;
	if (word[0] == '\0' && start == ctx->i)
		return (free(word), 1);
	if (!scn_emit_word(&ctx->head, word, ctx->err))
		return (0);
	return (1);
}

t_token	*scn_line(const char *line, t_shell *shell, t_err *err)
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
		while (ctx.line[ctx.i] && sup_is_space(ctx.line[ctx.i]))
			ctx.i++;
		if (!ctx.line[ctx.i])
			break ;
		if (ctx.line[ctx.i] == '|' || ctx.line[ctx.i] == '<'
			|| ctx.line[ctx.i] == '>')
		{
			if (!scan_operator_token(&ctx))
				return (NULL);
		}
		else if (!scan_word_token(&ctx))
			return (NULL);
	}
	return (ctx.head);
}
