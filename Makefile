CC = gcc
CFLAGS  = -g -Wall
all: client

app: download.c
	$(CC) $(CFLAGS) -o download download.c

client: clientTCP.c getip.c
	$(CC) $(CFLAGS) -o clientTCP clientTCP.c getip.c



clean:
	rm download getip clientTCP