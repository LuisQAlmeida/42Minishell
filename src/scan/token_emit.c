/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_emit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:55:20 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:55:20 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	scn_emit_word(t_token **head, char *word, t_err *err)
{
	t_token	*new_tok;

	new_tok = scn_token_new(TOK_WORD, word);
	if (!new_tok)
	{
		free(word);
		scn_token_clear(*head);
		*err = ERR_MALLOC;
		return (0);
	}
	scn_token_add(head, new_tok);
	return (1);
}

int	scn_emit_operator(t_token **head, t_toktype type, t_err *err)
{
	t_token	*new_tok;

	new_tok = scn_token_new(type, NULL);
	if (!new_tok)
	{
		scn_token_clear(*head);
		*err = ERR_MALLOC;
		return (0);
	}
	scn_token_add(head, new_tok);
	return (1);
}
