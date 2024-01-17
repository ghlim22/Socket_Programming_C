NAME = TCP_Server_4
CC = clang
CFLAGS = -Wall -Wextra -Werror
HEADER = 
LIBFT = 

SRCDIR = src

MANDFILE =	die_with_message.c \
			main.c \
			utils.c

MANDSRC = $(addprefix $(SRCDIR)/, $(MANDFILE))

MANDOBJ = $(MANDSRC:.c=.o)

OBJ = $(MANDOBJ)

%.o: %.c
	$(CC) $(CFLAGS) $(HEADER) -c $< -o $@

all: $(NAME)
.PHONY: all

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(NAME)

clean:
	rm -f $(OBJ)
.PHONY: clean

fclean: clean
	rm -f $(OBJ) $(NAME)
.PHONY: fclean

re:
	make fclean
	make all
.PHONY: re
