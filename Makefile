# Makefile #

VERSION = 0.0.1

SRC = bar.c
OBJ = $(SRC:.c=.o)

CC			= gcc
LINK    = gcc
TARGET  = loading-bar

all: loading-bar

loading-bar: $(OBJ)
	$(LINK) -DNCURSES_WIDECHAR -o $(TARGET) $(OBJ) -lncursesw

dist: clean
	mkdir loading-bar-$(VERSION) dist
	cp bar.c bar.h feed_the_bar.sh LICENSE loading-bar-$(VERSION)
	tar -cf - loading-bar-$(VERSION) | gzip > loading-bar-$(VERSION).tar.gz
	rm -rf loading-bar-$(VERSION)
	sha256sum loading-bar-$(VERSION).tar.gz > SHA256SUM
	gpg --detach-sign loading-bar-$(VERSION).tar.gz
	mv loading-bar-$(VERSION).tar.gz loading-bar-$(VERSION).tar.gz.sig SHA256SUM dist/

clean:
	rm -rf $(TARGET) $(OBJ) dist/ loading-bar-$(VERSION)

bar.o: bar.c bar.h
	$(CC) -DNCURSES_WIDECHAR -c -o bar.o bar.c -lncursesw
