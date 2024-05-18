NAME = client

CC 	   = 		cc
CFLAGS = 		-Wall -Wextra -Werror

SRC_DIR   = 	src
SRC_FILES = 	Practical.c \
				GetAddrInfo.c \
				TCPClientUtility.c \
				TCPEchoClient.c

SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJ = $(SRC:.c=.o)

all: $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(HEADER) -c $(CPPFLAGS) -o $@ $<

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(HEADER) $(LIB) -o $@ $^

clean:
	rm -f $(OBJ)

fclean:
	make clean
	rm -f $(NAME)

re: 
	make fclean
	make all

.PHONY: clean fclean re all bonus
