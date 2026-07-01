# **************************************************************************** #
#                                   minishell                                  #
# **************************************************************************** #

NAME = minishell

VAL_NAME = minishell_val

CC = cc
CFLAGS = -Wall -Wextra -Werror
RM = rm -f

INC_DIR = include
SRC_DIR = src
OBJ_DIR = obj
LIBFT_DIR = libft
TEST_DIR = tests
LIBFT = $(LIBFT_DIR)/libft.a

INCLUDES = -I$(INC_DIR) -I$(LIBFT_DIR)

SRCS = $(SRC_DIR)/main.c \
	$(SRC_DIR)/shell/main_loop.c \
	$(SRC_DIR)/shell/run_once.c \
	$(SRC_DIR)/builtins/builtin_dispatch.c \
	$(SRC_DIR)/builtins/builtin_parent.c \
	$(SRC_DIR)/builtins/builtin_echo.c \
	$(SRC_DIR)/builtins/builtin_pwd.c \
	$(SRC_DIR)/builtins/builtin_env.c \
	$(SRC_DIR)/builtins/builtin_export.c \
	$(SRC_DIR)/builtins/builtin_unset.c \
	$(SRC_DIR)/env/env_copy.c \
	$(SRC_DIR)/env/env_utils.c \
	$(SRC_DIR)/env/env_set.c \
	$(SRC_DIR)/env/env_unset.c \
	$(SRC_DIR)/expansion/expand_env.c \
	$(SRC_DIR)/expansion/expand_utils.c \
	$(SRC_DIR)/signals/signals.c \
	$(SRC_DIR)/signals/signals_exec.c \
	$(SRC_DIR)/parsing/tokenize_line.c \
	$(SRC_DIR)/parsing/read_word.c \
	$(SRC_DIR)/parsing/token_quoted.c \
	$(SRC_DIR)/parsing/token_utils.c \
	$(SRC_DIR)/parsing/token_free.c \
	$(SRC_DIR)/parsing/utils.c \
	$(SRC_DIR)/parsing/cmd.c \
	$(SRC_DIR)/parsing/cmd_free.c \
	$(SRC_DIR)/parsing/cmd_redir.c \
	$(SRC_DIR)/exec/exec_simple.c \
	$(SRC_DIR)/exec/exec_path.c \
	$(SRC_DIR)/exec/exec_wait.c \
	$(SRC_DIR)/exec/exec_redirs.c \
	$(SRC_DIR)/exec/exec_redir_one.c \
	$(SRC_DIR)/exec/exec_redir_only.c \
	$(SRC_DIR)/exec/exec_heredoc.c \
	$(SRC_DIR)/exec/exec_redir_prepare.c \
	$(SRC_DIR)/utils/ms_ctype.c

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

VAL_OBJS = $(OBJ_DIR)/tests/parsing/token_parser_test.o \
	$(OBJ_DIR)/parsing/tokenize_line.o \
	$(OBJ_DIR)/parsing/read_word.o \
	$(OBJ_DIR)/parsing/token_quoted.o \
	$(OBJ_DIR)/parsing/token_utils.o \
	$(OBJ_DIR)/parsing/token_free.o \
	$(OBJ_DIR)/parsing/utils.o \
	$(OBJ_DIR)/parsing/cmd.o \
	$(OBJ_DIR)/parsing/cmd_free.o \
	$(OBJ_DIR)/utils/ms_ctype.o

all: $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -lreadline -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

valgrind_test: $(LIBFT) $(VAL_OBJS)
	$(CC) $(CFLAGS) $(VAL_OBJS) $(LIBFT) -o $(VAL_NAME)

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	$(RM) -rf $(OBJ_DIR)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	$(RM) $(NAME)
	$(RM) $(VAL_NAME)

re: fclean all

.PHONY: all clean fclean re valgrind_test
