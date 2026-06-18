NAME = codexion

CC = cc

CFLAGS = -Wall -Wextra -Werror -pthread

SRC = \
src/main.c \
src/time.c \
src/parsing_utils.c \
src/parsing.c \
src/init/init.c \
src/init/start.c \
src/init/destroy.c \
src/coder/coder.c \
src/coder/routine.c \
src/monitor/monitor.c \
src/utils/stop.c \
src/print.c \
src/dongle/dongle.c

OBJ = $(SRC:.c=.o)

INCLUDES = -Iinclude

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
