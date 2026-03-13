#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>

typedef enum e_toktype
{
    TOK_WORD,
    TOK_PIPE,       // |
    TOK_REDIR_IN,   // <
    TOK_REDIR_OUT,  // >
    TOK_HEREDOC,    // <<
    TOK_APPEND      // >>
}   t_toktype;

typedef struct s_token
{
    t_toktype       type;
    char            *value;
    struct s_token  *next;
}   t_token;

t_token *token_new(t_toktype type, char *value);
void    token_add_back(t_token **lst, t_token *new_tok);
void    free_tokens(t_token *lst);

#endif
