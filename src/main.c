#include "minishell.h"

static void print_tokens(t_token *list)
{
    while (list)
    {
        printf("type=%d value=%s\n",
            list->type,
            list->value ? list->value : "(null)");
        list = list->next;
    }
}

int main(int ac, char **av)
{
    t_token *list;
    t_err    err;

    if (ac != 2)
    {
        printf("Usage: %s \"command line\"\n", av[0]);
        return (1);
    }
    list = tokenize_line(av[1], &err);
    if (!list && err == ERR_UNCLOSED_QUOTE)
        printf("minishell: syntax error: unclosed quote\n");
    else if (!list && err == ERR_MALLOC)
        printf("minishell: error: malloc failed\n");
    else
        print_tokens(list);
    free_tokens(list);
    return (0);
}