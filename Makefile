CC = gcc
CFLAGS  = -g -Wall
all: download

download: download.c
	$(CC) $(CFLAGS) -o download download.c

clean:
	rm download