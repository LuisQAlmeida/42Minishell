/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpedro-g <jpedro-g@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:58:04 by jpedro-g          #+#    #+#             */
/*   Updated: 2026/07/09 15:58:04 by jpedro-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <stdlib.h>
# include <unistd.h>
# include <stdint.h>

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

/* ************************************************************************** */
/*                                  Memory                                    */
/* ************************************************************************** */

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

/* ************************************************************************** */
/*                                Conversion                                  */
/* ************************************************************************** */

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

#endif
