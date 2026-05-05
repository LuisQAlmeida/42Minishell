#include "minishell.h"
#include <string.h>

t_token *token_new(t_toktype type, char *value)
{
    t_token *t;

    t = malloc(sizeof(t_token));
    if (!t)
        return (NULL);
    t->type = type;
    t->value = value;
    t->next = NULL;
    return (t);
}

static  t_token *last_token(t_token *lst)
{
    while (lst && lst->next)
        lst = lst->next;
    return (lst);
}

void    token_add_back(t_token **lst, t_token *new_tok)
{
    t_token *last;

    if (!new_tok)
        return ;
    if (!*lst)
    {
        *lst = new_tok;
        return ;
    }
    last = last_token(*lst);
    last->next = new_tok;
}

void    free_tokens(t_token *lst)
{
    t_token *next;

    while (lst)
    {
        next = lst->next;
        free(lst->value);
        free(lst);
        lst = next;
    }
}