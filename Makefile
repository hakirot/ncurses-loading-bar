# Makefile #

VERSION = 0.1.0

SRC = bar.c
OBJ = $(SRC:.c=.o)

CC			= gcc
LINK    = gcc
LFLAGS  = -lncursesw -DNCURSES_WIDECHAR=1
TARGET  = bar

all: bar

bar: $(OBJ)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJ)

clean:
	rm -f bar $(OBJ)

bar.o: bar.c bar.h
	$(CC) -c -o bar.o bar.c

