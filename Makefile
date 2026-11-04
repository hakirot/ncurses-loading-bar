# Makefile #

VERSION = 0.1.0

SRC = bar.c
OBJ = $(SRC:.c=.o)

CC			= gcc
LINK    = gcc
TARGET  = loading-bar

all: loading-bar

loading-bar: $(OBJ)
	$(LINK) -DNCURSES_WIDECHAR -o $(TARGET) $(OBJ) -lncursesw

clean:
	rm -f $(TARGET) $(OBJ)

bar.o: bar.c bar.h
	$(CC) -DNCURSES_WIDECHAR -c -o bar.o bar.c -lncursesw
