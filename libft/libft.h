#ifndef LIBFT_H
# define LIBFT_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 10
# endif

# ifndef MAX_BUFFER_SIZE
#  define MAX_BUFFER_SIZE 10000000
# endif

# include <stdlib.h>
# include <unistd.h>
# include <stdint.h>
# include <stdarg.h>

/**
 * @brief Singly linked list node.
 *
 * Stores generic content and a pointer to the next node.
 */
typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}	t_list;

/* ************************************************************************** */
/*                                Character                                   */
/* ************************************************************************** */

/**
 * @brief Checks whether a character is alphabetic.
 *
 * @param c Character to test.
 * @return 1 if alphabetic, 0 otherwise.
 */
int		ft_isalpha(int c);

/**
 * @brief Checks whether a character is a digit.
 *
 * @param c Character to test.
 * @return 1 if digit, 0 otherwise.
 */
int		ft_isdigit(int c);

/**
 * @brief Checks whether a character is alphanumeric.
 *
 * @param c Character to test.
 * @return 1 if alphabetic or digit, 0 otherwise.
 */
int		ft_isalnum(int c);

/**
 * @brief Checks whether a character belongs to the ASCII table.
 *
 * @param c Character to test.
 * @return 1 if ASCII, 0 otherwise.
 */
int		ft_isascii(int c);

/**
 * @brief Checks whether a character is printable.
 *
 * @param c Character to test.
 * @return 1 if printable, 0 otherwise.
 */
int		ft_isprint(int c);

/**
 * @brief Converts a lowercase character to uppercase.
 *
 * @param c Character to convert.
 * @return Uppercase equivalent if possible, otherwise c unchanged.
 */
int		ft_toupper(int c);

/**
 * @brief Converts an uppercase character to lowercase.
 *
 * @param c Character to convert.
 * @return Lowercase equivalent if possible, otherwise c unchanged.
 */
int		ft_tolower(int c);

/* ************************************************************************** */
/*                                  Memory                                    */
/* ************************************************************************** */

/**
 * @brief Fills memory with a byte value.
 *
 * @param s Memory area to fill.
 * @param c Byte value to write.
 * @param n Number of bytes to write.
 * @return Pointer to the memory area.
 */
void	*ft_memset(void *s, int c, size_t n);

/**
 * @brief Sets a memory area to zero.
 *
 * @param s Memory area to clear.
 * @param n Number of bytes to clear.
 */
void	ft_bzero(void *s, size_t n);

/**
 * @brief Copies memory from src to dest.
 *
 * Undefined behavior occurs if memory areas overlap.
 *
 * @param dest Destination memory area.
 * @param src Source memory area.
 * @param n Number of bytes to copy.
 * @return Pointer to dest.
 */
void	*ft_memcpy(void *dest, const void *src, size_t n);

/**
 * @brief Copies memory safely even when areas overlap.
 *
 * @param dest Destination memory area.
 * @param src Source memory area.
 * @param n Number of bytes to copy.
 * @return Pointer to dest.
 */
void	*ft_memmove(void *dest, const void *src, size_t n);

/**
 * @brief Searches memory for a byte value.
 *
 * @param s Memory area to search.
 * @param c Byte value to find.
 * @param n Number of bytes to inspect.
 * @return Pointer to the first match, or NULL if not found.
 */
void	*ft_memchr(const void *s, int c, size_t n);

/**
 * @brief Compares two memory areas.
 *
 * @param s1 First memory area.
 * @param s2 Second memory area.
 * @param n Number of bytes to compare.
 * @return Difference between first differing bytes, or 0 if equal.
 */
int		ft_memcmp(const void *s1, const void *s2, size_t n);

/**
 * @brief Allocates zero-initialized memory for an array.
 *
 * Checks for multiplication overflow before allocation.
 *
 * @param nmemb Number of elements.
 * @param size Size of each element.
 * @return Allocated memory, or NULL on failure.
 */
void	*ft_calloc(size_t nmemb, size_t size);

/* ************************************************************************** */
/*                                  Strings                                   */
/* ************************************************************************** */

/**
 * @brief Calculates the length of a string.
 *
 * @param s String to measure.
 * @return Number of characters before the null terminator.
 */
size_t	ft_strlen(const char *s);

/**
 * @brief Copies a string into a sized buffer.
 *
 * @param dst Destination buffer.
 * @param src Source string.
 * @param size Total size of destination buffer.
 * @return Length of src.
 */
size_t	ft_strlcpy(char *dst, const char *src, size_t size);

/**
 * @brief Appends a string into a sized buffer.
 *
 * @param dst Destination buffer.
 * @param src Source string.
 * @param size Total size of destination buffer.
 * @return Length of the string it tried to create.
 */
size_t	ft_strlcat(char *dst, const char *src, size_t size);

/**
 * @brief Compares two strings up to n characters.
 *
 * @param s1 First string.
 * @param s2 Second string.
 * @param n Maximum number of characters to compare.
 * @return Difference between first differing characters, or 0.
 */
int		ft_strncmp(const char *s1, const char *s2, size_t n);

/**
 * @brief Finds the first occurrence of a character in a string.
 *
 * @param s String to search.
 * @param c Character to find.
 * @return Pointer to the match, or NULL if not found.
 */
char	*ft_strchr(const char *s, int c);

/**
 * @brief Finds the last occurrence of a character in a string.
 *
 * @param s String to search.
 * @param c Character to find.
 * @return Pointer to the match, or NULL if not found.
 */
char	*ft_strrchr(const char *s, int c);

/**
 * @brief Locates a substring inside a limited string range.
 *
 * @param big String to search in.
 * @param little Substring to find.
 * @param len Maximum number of characters to search.
 * @return Pointer to the first match, or NULL if not found.
 */
char	*ft_strnstr(const char *big, const char *little, size_t len);

/**
 * @brief Duplicates a string into newly allocated memory.
 *
 * The caller is responsible for freeing the returned string.
 *
 * @param s String to duplicate.
 * @return Newly allocated duplicate, or NULL on failure.
 */
char	*ft_strdup(const char *s);

/**
 * @brief Extracts a substring into newly allocated memory.
 *
 * The caller is responsible for freeing the returned string.
 *
 * @param s Source string.
 * @param start Starting index.
 * @param len Maximum substring length.
 * @return Newly allocated substring, or NULL on failure.
 */
char	*ft_substr(char const *s, unsigned int start, size_t len);

/**
 * @brief Joins two strings into newly allocated memory.
 *
 * The caller is responsible for freeing the returned string.
 *
 * @param s1 First string.
 * @param s2 Second string.
 * @return Newly allocated joined string, or NULL on failure.
 */
char	*ft_strjoin(char const *s1, char const *s2);

/**
 * @brief Trims characters from the beginning and end of a string.
 *
 * The caller is responsible for freeing the returned string.
 *
 * @param s1 String to trim.
 * @param set Characters to remove.
 * @return Newly allocated trimmed string, or NULL on failure.
 */
char	*ft_strtrim(char const *s1, char const *set);

/**
 * @brief Splits a string into an array using a delimiter.
 *
 * The returned array is NULL-terminated. The caller owns the array and
 * every string inside it.
 *
 * @param s String to split.
 * @param c Delimiter character.
 * @return NULL-terminated array, or NULL on allocation failure.
 */
char	**ft_split(char const *s, char c);

/**
 * @brief Creates a new string by applying a function to each character.
 *
 * The caller is responsible for freeing the returned string.
 *
 * @param s Source string.
 * @param f Function applied to each character.
 * @return Newly allocated mapped string, or NULL on failure.
 */
char	*ft_strmapi(char const *s, char (*f)(unsigned int, char));

/**
 * @brief Applies a function to each character of a string in place.
 *
 * @param s String to modify.
 * @param f Function applied to each character address.
 */
void	ft_striteri(char *s, void (*f)(unsigned int, char*));

/* ************************************************************************** */
/*                                Conversion                                  */
/* ************************************************************************** */

/**
 * @brief Converts a string to an integer.
 *
 * @param nptr String to convert.
 * @return Converted integer value.
 */
int		ft_atoi(const char *nptr);

/**
 * @brief Converts an integer to a newly allocated string.
 *
 * The caller is responsible for freeing the returned string.
 *
 * @param n Integer to convert.
 * @return Newly allocated string, or NULL on failure.
 */
char	*ft_itoa(int n);

/* ************************************************************************** */
/*                                  Output                                    */
/* ************************************************************************** */

/**
 * @brief Writes a character to a file descriptor.
 *
 * @param c Character to write.
 * @param fd File descriptor.
 */
void	ft_putchar_fd(char c, int fd);

/**
 * @brief Writes a string to a file descriptor.
 *
 * @param s String to write.
 * @param fd File descriptor.
 */
void	ft_putstr_fd(char *s, int fd);

/**
 * @brief Writes a string followed by a newline to a file descriptor.
 *
 * @param s String to write.
 * @param fd File descriptor.
 */
void	ft_putendl_fd(char *s, int fd);

/**
 * @brief Writes an integer to a file descriptor.
 *
 * @param n Integer to write.
 * @param fd File descriptor.
 */
void	ft_putnbr_fd(int n, int fd);

/* ************************************************************************** */
/*                               Linked lists                                 */
/* ************************************************************************** */

/**
 * @brief Creates a new list node.
 *
 * @param content Content stored in the node.
 * @return New node, or NULL on allocation failure.
 */
t_list	*ft_lstnew(void *content);

/**
 * @brief Adds a node to the front of a list.
 *
 * @param lst Address of the list head.
 * @param new Node to add.
 */
void	ft_lstadd_front(t_list **lst, t_list *new);

/**
 * @brief Counts the number of nodes in a list.
 *
 * @param lst List head.
 * @return Number of nodes.
 */
int		ft_lstsize(t_list *lst);

/**
 * @brief Returns the last node of a list.
 *
 * @param lst List head.
 * @return Last node, or NULL if the list is empty.
 */
t_list	*ft_lstlast(t_list *lst);

/**
 * @brief Adds a node to the end of a list.
 *
 * @param lst Address of the list head.
 * @param new Node to add.
 */
void	ft_lstadd_back(t_list **lst, t_list *new);

/**
 * @brief Deletes a single list node.
 *
 * Applies del to the content before freeing the node.
 *
 * @param lst Node to delete.
 * @param del Function used to free the content.
 */
void	ft_lstdelone(t_list *lst, void (*del)(void *));

/**
 * @brief Deletes and clears an entire list.
 *
 * Applies del to each node content before freeing all nodes.
 *
 * @param lst Address of the list head.
 * @param del Function used to free each content.
 */
void	ft_lstclear(t_list **lst, void (*del)(void *));

/**
 * @brief Applies a function to each node content.
 *
 * @param lst List head.
 * @param f Function applied to each content.
 */
void	ft_lstiter(t_list *lst, void (*f)(void *));

/**
 * @brief Creates a new list by mapping a function over another list.
 *
 * The del function is used to clean allocated content on failure.
 *
 * @param lst Source list.
 * @param f Function used to transform each content.
 * @param del Function used to free content if allocation fails.
 * @return New list, or NULL on failure.
 */
t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *));

/* ************************************************************************** */
/*                                  Printf                                    */
/* ************************************************************************** */

/**
 * @brief Prints formatted output to standard output.
 *
 * Supports a subset of printf conversions used in the 42 ft_printf project.
 *
 * @param format Format string.
 * @return Number of printed characters, or a negative value on error.
 */
int		ft_printf(const char *format, ...);

/**
 * @brief Writes a character to standard output.
 *
 * @param c Character to write.
 * @return Number of bytes written.
 */
int		ft_putchar_a(char c);

/**
 * @brief Writes a string to standard output.
 *
 * Prints "(null)" if the string is NULL.
 *
 * @param s String to write.
 * @return Number of characters printed.
 */
int		ft_putstr_a(const char *s);

/**
 * @brief Prints an unsigned number in base 10 or base 16.
 *
 * @param num Number to print.
 * @param c Format selector.
 * @return Number of printed characters.
 */
int		ft_putnum(unsigned long long num, char c);

/**
 * @brief Dispatches printf format conversion.
 *
 * @param arg_list Argument list.
 * @param c Conversion specifier.
 * @return Number of printed characters.
 */
int		ft_print_format(va_list arg_list, char c);

/* ************************************************************************** */
/*                              Get Next Line                                 */
/* ************************************************************************** */

/**
 * @brief Reads the next line from a file descriptor.
 *
 * The caller is responsible for freeing the returned line.
 *
 * @param fd File descriptor to read from.
 * @return Next line, or NULL on EOF/error.
 */
char	*get_next_line(int fd);

/**
 * @brief Calculates length up to and including newline if present.
 *
 * @param s String to measure.
 * @return Length until newline or null terminator.
 */
size_t	ft_strlen_gnl(const char *s);

/**
 * @brief Joins the current line with the GNL buffer.
 *
 * Frees the previous line and returns a newly allocated string.
 *
 * @param line Current accumulated line.
 * @param buffer Static read buffer.
 * @return Newly allocated joined line, or NULL on failure.
 */
char	*ft_strjoin_gnl(char *line, char *buffer);

/**
 * @brief Copies line and buffer content into a destination line.
 *
 * @param nxt_line Destination line.
 * @param line Current accumulated line.
 * @param buffer Buffer content to append.
 * @param len_buffer Number of buffer bytes to append.
 */
void	ft_concat(char *nxt_line, char *line, char *buffer, size_t len_buffer);

/**
 * @brief Removes consumed content from the GNL static buffer.
 *
 * Keeps only the content after the first newline.
 *
 * @param buffer Static buffer to update.
 */
void	ft_excess(char *buffer);

#endif
