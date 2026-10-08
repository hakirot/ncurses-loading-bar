# Makefile #

VERSION = 0.1.0

SRC = bar.c
OBJ = $(SRC:.c=.o)

CC			= gcc
LINK    = gcc
LFLAGS  = -lncursesw -DNCURSES_WIDECHAR=1
TARGET  = loading-bar

all: loading-bar

loading-bar: $(OBJ)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJ)

clean:
	rm -f $(TARGET) $(OBJ)

bar.o: bar.c bar.h
	$(CC) $(LFLAGS) -c -o bar.o bar.c

