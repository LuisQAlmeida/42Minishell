/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   variable_name.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:55:00 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:55:00 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exp_name_start(char c)
{
	return (ft_isalpha(c) || c == '_');
}

int	exp_name_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}
