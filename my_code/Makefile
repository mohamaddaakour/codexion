# the name of the executable file
NAME = codexion

# the compiler to use is cc
CC = cc

# the flags we want
# -Wall -Wextra -Werror treatng warnings as errors
# -pthread enable POSIX threads support (pthread)
CFLAGS = -Wall -Wextra -Werror -pthread

# the files .c we have
SRC = src/utils.c src/main.c src/coder.c src/routine.c src/dongle.c src/destroy.c \
src/heap.c src/init.c

# take all .c files and create also .o files for them
OBJ = $(SRC:.c=.o)

# we tell it to search for .h header file
INCLUDES = -I include

# running make will run this
all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# make clean will remove the object files .o
clean:
	rm -f $(OBJ)

# make fclean will remove all .o and teh executable file
fclean: clean
	rm -f $(NAME)

# make re will remove .o and excutable file and run make after that
re: fclean all

# make .PHONY will run all these
.PHONY: all clean fclean re
