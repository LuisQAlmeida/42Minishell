/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   line_execution.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:55:32 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:55:32 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	print_error(const char *msg)
{
	ft_putendl_fd((char *)msg, STDERR_FILENO);
	return (1);
}

static int	report_parse_error(t_token *list, t_err err)
{
	scn_token_clear(list);
	if (err == ERR_MALLOC)
		return (print_error("minishell: error: malloc failed"));
	if (err == ERR_SYNTAX)
		return (print_error("minishell: syntax error near unexpected token"));
	return (1);
}

static int	dispatch_commands(t_cmd *cmd, t_shell *shell, t_token *tokens)
{
	if (grm_count(cmd) == 1)
		return (exe_simple(cmd, shell, tokens));
	return (exe_pipeline(cmd, shell, tokens));
}

int	ses_execute_line(const char *line, t_shell *shell)
{
	t_token	*list;
	t_cmd	*cmd;
	t_err	err;

	list = scn_line(line, shell, &err);
	if (!list && err == ERR_UNCLOSED_QUOTE)
		return (print_error("minishell: syntax error: unclosed quote"));
	if (!list && err == ERR_MALLOC)
		return (print_error("minishell: error: malloc failed"));
	if (!list && err == ERR_NONE)
		return (0);
	cmd = grm_pipeline(list, &err);
	if (!cmd)
		return (report_parse_error(list, err));
	shell->last_status = dispatch_commands(cmd, shell, list);
	grm_clear(cmd);
	scn_token_clear(list);
	return (shell->last_status);
}
