NAME = codexion

CC = cc

CFLAGS = -Wall -Wextra -Werror -pthread

SRC = src/time.c src/utils.c src/main.c src/routine.c src/dongle.c src/destroy.c src/heap.c src/init.c src/dongle_helpers.c src/heap_helpers.c src/routine_helpers.c

OBJ = $(SRC:.c=.o)

INCLUDES = -I include

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
