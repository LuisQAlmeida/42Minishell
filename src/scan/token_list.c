/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_list.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:55:23 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:55:23 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_token	*token_last(t_token *lst)
{
	while (lst && lst->next)
		lst = lst->next;
	return (lst);
}

t_token	*scn_token_new(t_toktype type, char *value)
{
	t_token	*t;

	t = malloc(sizeof(t_token));
	if (!t)
		return (NULL);
	t->type = type;
	t->value = value;
	t->next = NULL;
	return (t);
}

void	scn_token_add(t_token **lst, t_token *new_tok)
{
	t_token	*last;

	if (!new_tok)
		return ;
	if (!*lst)
	{
		*lst = new_tok;
		return ;
	}
	last = token_last(*lst);
	last->next = new_tok;
}
