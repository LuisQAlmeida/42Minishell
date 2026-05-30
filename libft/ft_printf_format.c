#include "libft.h"

static int	print_int(va_list arg_list)
{
	int					n;
	int					print_len;
	unsigned long long	num;

	n = va_arg(arg_list, int);
	print_len = 0;
	if (n < 0)
	{
		print_len += ft_putchar_a('-');
		num = -(long)n;
	}
	else
		num = n;
	return (print_len + ft_putnum(num, 'd'));
}

static int	print_ptr(va_list arg_list)
{
	int		print_len;
	void	*ptr;

	print_len = 0;
	ptr = va_arg(arg_list, void *);
	if (!ptr)
		return (ft_putstr_a("(nil)"));
	print_len += ft_putstr_a("0x");
	return (print_len + ft_putnum((unsigned long long)ptr, 'x'));
}

int	ft_print_format(va_list arg_list, char c)
{
	if (c == 'c')
		return (ft_putchar_a(va_arg(arg_list, int)));
	if (c == 's')
		return (ft_putstr_a(va_arg(arg_list, char *)));
	if (c == 'd' || c == 'i')
		return (print_int(arg_list));
	if (c == 'u' || c == 'x' || c == 'X')
		return (ft_putnum(va_arg(arg_list, unsigned int), c));
	if (c == 'p')
		return (print_ptr(arg_list));
	if (c == '%')
		return (ft_putchar_a('%'));
	return (0);
}
