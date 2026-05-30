#include "minishell.h"

int	ms_isspace(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}
