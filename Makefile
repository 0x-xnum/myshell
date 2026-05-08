CC     = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC    = src/shell.c src/core.c src/builtins.c src/history.c
OBJ    = $(SRC:.c=.o)
BIN    = mysh

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean
