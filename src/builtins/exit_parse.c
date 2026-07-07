#include "minishell.h"

static int	exit_parse_sign(const char *str, int *i)
{
	int	sign;

	sign = 1;
	if (str[*i] == '-' || str[*i] == '+')
	{
		if (str[*i] == '-')
			sign = -1;
		(*i)++;
	}
	return (sign);
}

static int	exit_add_digit(unsigned long long *num, char c, int sign)
{
	unsigned long long	limit;
	unsigned long long	digit;

	if (sign < 0)
		limit = 9223372036854775808ULL;
	else
		limit = 9223372036854775807ULL;
	digit = c - '0';
	if (*num > limit / 10)
		return (0);
	if (*num == limit / 10 && digit > limit % 10)
		return (0);
	*num = (*num * 10) + digit;
	return (1);
}

int	blt_exit_parse(const char *str, long long *value)
{
	unsigned long long	num;
	int					i;
	int					sign;

	i = 0;
	num = 0;
	while (sup_is_space(str[i]))
		i++;
	sign = exit_parse_sign(str, &i);
	if (!ft_isdigit(str[i]))
		return (0);
	while (ft_isdigit(str[i]))
	{
		if (!exit_add_digit(&num, str[i], sign))
			return (0);
		i++;
	}
	while (sup_is_space(str[i]))
		i++;
	if (str[i] != '\0')
		return (0);
	*value = (long long)(num * sign);
	return (1);
}
