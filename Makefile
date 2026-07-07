# **************************************************************************** #
#                                   minishell                                  #
# **************************************************************************** #

NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror
RM = rm -f

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include
LIBFT_DIR = libft

LIBFT = $(LIBFT_DIR)/libft.a
INCLUDES = -I$(INC_DIR) -I$(LIBFT_DIR)

SESSION_DIR = $(SRC_DIR)/session
SCAN_DIR = $(SRC_DIR)/scan
GRAMMAR_DIR = $(SRC_DIR)/grammar
EXPAND_DIR = $(SRC_DIR)/expand
EXEC_DIR = $(SRC_DIR)/exec
BUILTINS_DIR = $(SRC_DIR)/builtins
STATE_DIR = $(SRC_DIR)/state
SIGNALS_DIR = $(SRC_DIR)/signals
SUPPORT_DIR = $(SRC_DIR)/support

MAIN_FILES = \
	main.c

SESSION_FILES = \
	prompt_loop.c \
	line_execution.c

SCAN_FILES = \
	line_scan.c \
	word_scan.c \
	quote_scan.c \
	token_emit.c \
	token_list.c \
	token_memory.c

GRAMMAR_FILES = \
	command_chain.c \
	command_build.c \
	redirection_build.c \
	command_memory.c

EXPAND_FILES = \
	variables.c \
	variable_name.c \
	variable_value.c

EXEC_FILES = \
	command_simple.c \
	pipeline_wait.c \
	pipeline.c \
	pipeline_child.c \
	child_command.c \
	wait_child.c \
	redir_prepare.c \
	redir_apply.c \
	redir_open.c \
	redir_only.c \
	heredoc.c \
	heredoc_read.c \
	heredoc_terminal.c \
	path_lookup.c \
	stdio_backup.c

BUILTINS_FILES = \
	dispatch.c \
	parent.c \
	echo.c \
	pwd.c \
	env.c \
	export.c \
	unset.c \
	cd.c \
	exit.c \
	exit_parse.c \
	identifier.c

STATE_FILES = \
	env_copy.c \
	env_entry.c \
	env_lookup.c \
	env_set.c \
	env_unset.c

SIGNALS_FILES = \
	input_signals.c \
	process_signals.c

SUPPORT_FILES = \
	shell_support.c

SRCS = \
	$(addprefix $(SRC_DIR)/,$(MAIN_FILES)) \
	$(addprefix $(SESSION_DIR)/,$(SESSION_FILES)) \
	$(addprefix $(SCAN_DIR)/,$(SCAN_FILES)) \
	$(addprefix $(GRAMMAR_DIR)/,$(GRAMMAR_FILES)) \
	$(addprefix $(EXPAND_DIR)/,$(EXPAND_FILES)) \
	$(addprefix $(EXEC_DIR)/,$(EXEC_FILES)) \
	$(addprefix $(BUILTINS_DIR)/,$(BUILTINS_FILES)) \
	$(addprefix $(STATE_DIR)/,$(STATE_FILES)) \
	$(addprefix $(SIGNALS_DIR)/,$(SIGNALS_FILES)) \
	$(addprefix $(SUPPORT_DIR)/,$(SUPPORT_FILES))

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -lreadline -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	$(RM) -rf $(OBJ_DIR)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
