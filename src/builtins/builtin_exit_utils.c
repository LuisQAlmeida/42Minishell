#include "minishell.h"

static int	parse_sign(const char *str, int *i)
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

static int	add_digit(unsigned long long *num, char c, int sign)
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

int	parse_exit_number(const char *str, long long *value)
{
	unsigned long long	num;
	int					i;
	int					sign;

	i = 0;
	num = 0;
	while (ms_isspace(str[i]))
		i++;
	sign = parse_sign(str, &i);
	if (!ft_isdigit(str[i]))
		return (0);
	while (ft_isdigit(str[i]))
	{
		if (!add_digit(&num, str[i], sign))
			return (0);
		i++;
	}
	while (ms_isspace(str[i]))
		i++;
	if (str[i] != '\0')
		return (0);
	*value = (long long)(num * sign);
	return (1);
}
