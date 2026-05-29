#include "minishell.h"
#include <ctype.h>

char    *ms_strjoin_free(char *s1, char *s2)
{
    size_t  len1;
    size_t  len2;
    size_t  i;
    char    *res;

    len1 = 0;
    if (s1)
        len1 = ft_strlen(s1);
    len2 = 0;
    if (s2)
        len2 = ft_strlen(s2);
    res = malloc(len1 + len2 + 1);
    if (!res)
        return (NULL);
    i = 0;
    while (i < len1)
    {
        res[i] = s1[i];
        i++;
    }
    i = 0;
    while (i < len2)
    {
        res[len1 + i] = s2[i];
        i++;
    }
    res[len1 + len2] = '\0';
    if (s1)
        free(s1);
    return (res);
}

char    *read_single_quoted(const char *line, size_t *i, t_err *err)
{
    size_t  start;
    char    *seg;

    (*i)++;
    start = *i;
    while (line[*i] && line[*i] != '\'')
        (*i)++;
    if (!line[*i])
    {
        *err = ERR_UNCLOSED_QUOTE;
        return (NULL);
    }
    seg = ft_substr(line, start, *i - start);
    if (!seg)
        *err = ERR_MALLOC;
    else
        (*i)++;
    return (seg);
}

char    *read_double_quoted(const char *line, size_t *i, t_err *err)
{
    size_t  start;
    char    *seg;

    (*i)++;
    start = *i;
    while (line[*i] && line[*i] != '"')
        (*i)++;
    if (!line[*i])
    {
        *err = ERR_UNCLOSED_QUOTE;
        return (NULL);
    }
    seg = ft_substr(line, start, *i - start);
    if (!seg)
        *err = ERR_MALLOC;
    else
        (*i)++;
    return (seg);
}