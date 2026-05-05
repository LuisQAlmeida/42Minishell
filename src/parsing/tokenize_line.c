#include "minishell.h"
#include <ctype.h>

static char *ms_strjoin_free(char *s1, char *s2)
{
    size_t  len1;
    size_t  len2;
    size_t  i;
    char    *res;

    len1 = s1 ? ft_strlen(s1) : 0;
    len2 = s2 ? ft_strlen(s2) : 0;
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
    free(s1);
    return (res);
}

static char *read_single_quoted(const char *line, size_t *i, t_err *err)
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

static char *read_plain_segment(const char *line, size_t *i, t_err *err)
{
    size_t  start;
    char    *seg;

    start = *i;
    while (line[*i]
        && !isspace((unsigned char)line[*i])
        && line[*i] != '\'')
        (*i)++;
    seg = ft_substr(line, start, *i - start);
    if (!seg)
        *err = ERR_MALLOC;
    return (seg);
}

static char *read_word(const char *line, size_t *i, t_err *err)
{
    char    *word;
    char    *seg;

    word = NULL;
    while (line[*i] && !isspace((unsigned char)line[*i]))
    {
        if (line[*i] == '\'')
            seg = read_single_quoted(line, i, err);
        else
            seg = read_plain_segment(line, i, err);
        if (!seg || *err != ERR_NONE)
            return (word);
        word = ms_strjoin_free(word, seg);
        if (!word)
        {
            free(seg);
            *err = ERR_MALLOC;
            return (NULL);
        }
    }
    return (word);
}

t_token *tokenize_line(const char *line, t_err *err)
{
    t_token *head;
    t_token *new_tok;
    char    *word;
    size_t  i;

    head = NULL;
    *err = ERR_NONE;
    i = 0;
    while (line[i])
    {
        while (line[i] && isspace((unsigned char)line[i]))
            i++;
        if (!line[i])
            break ;
        word = read_word(line, &i, err);
        if (!word || *err != ERR_NONE)
        {
            free_tokens(head);
            return (NULL);
        }
        new_tok = token_new(TOK_WORD, word);
        if (!new_tok)
        {
            free(word);
            free_tokens(head);
            *err = ERR_MALLOC;
            return (NULL);
        }
        token_add_back(&head, new_tok);
    }
    return (head);
}