#include "libft.h"

int	ft_putchar_a(char c)
{
	return (write(1, &c, 1));
}

int	ft_putstr_a(const char *s)
{
	int	i;

	i = 0;
	if (!s)
		return (write(1, "(null)", 6));
	while (s[i])
		write(1, &s[i++], 1);
	return (i);
}

int	ft_putnum(unsigned long long num, char c)
{
	char	*base;
	int		base_len;
	int		print_len;

	print_len = 0;
	if (c == 'X')
		base = "0123456789ABCDEF";
	else
		base = "0123456789abcdef";
	if (c == 'd' || c == 'u')
		base_len = 10;
	else
		base_len = 16;
	if (num >= (unsigned int)base_len)
		print_len += ft_putnum(num / base_len, c);
	print_len += ft_putchar_a(base[num % base_len]);
	return (print_len);
}
