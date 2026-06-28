CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
SRCS = main.c utils.c
OBJS = $(SRCS:.c=.o)
NAME = codexion

all : $(OBJS)
	$(CC) $(OBJS) -o $(NAME)
clean :
	rm -rf $(OBJS)
fclean : clean
	rm -rf $(NAME)
$(NAME): all
re: fclean all
debug:
	$(CC) *.c -g -pthread 
