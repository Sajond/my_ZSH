CC = gcc
CFLAGS = -Wall -Wextra -Werror

SRC = main.c my_ZSH.c
OBJ = $(SRC:.c=.o)

NAME = my_zsh

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c Zsh.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean $(NAME)

.PHONY: clean fclean re