#include "minishell.h"

int	exp_name_start(char c)
{
	return (ft_isalpha(c) || c == '_');
}

int	exp_name_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}
