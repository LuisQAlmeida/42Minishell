/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_terminal.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:54:30 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:54:30 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exe_heredoc_save_terminal(struct termios *saved)
{
	if (tcgetattr(STDIN_FILENO, saved) != 0)
		return (0);
	return (1);
}

void	exe_heredoc_restore(struct termios *saved, int has_saved)
{
	if (has_saved)
		tcsetattr(STDIN_FILENO, TCSANOW, saved);
}
