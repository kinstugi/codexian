NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
HDRS = $(wildcard $(INC_DIR)/*.h)

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HDRS)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
