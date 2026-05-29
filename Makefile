# **************************************************************************** #
#                                   minishell                                  #
# **************************************************************************** #

NAME = minishell

VAL_NAME = minishell_val

VAL_SRCS = $(SRC_DIR)/tests/token_parser_test.c \
           $(SRC_DIR)/parsing/tokenize_line.c \
           $(SRC_DIR)/parsing/token_utils.c \
           $(SRC_DIR)/parsing/utils.c \
           $(SRC_DIR)/parsing/cmd.c

VAL_OBJS = $(VAL_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC = cc
CFLAGS = -Wall -Wextra -Werror
RM = rm -f

INC_DIR = include
SRC_DIR = src
OBJ_DIR = obj

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/parsing/tokenize_line.c \
       $(SRC_DIR)/parsing/token_utils.c \
       $(SRC_DIR)/parsing/utils.c \
       $(SRC_DIR)/parsing/cmd.c \
       $(SRC_DIR)/exec/exec_simple.c

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -I$(INC_DIR) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)
	$(RM) -rf $(OBJ_DIR)

valgrind_test: $(VAL_OBJS)
	$(CC) $(CFLAGS) $(VAL_OBJS) -I$(INC_DIR) -o $(VAL_NAME)

re: fclean all

.PHONY: all clean fclean re
