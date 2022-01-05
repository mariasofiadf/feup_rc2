CC = gcc
CFLAGS  = -g -Wall
all: client getip

app: download.c
	$(CC) $(CFLAGS) -o download download.c

client: clientTCP.c
	$(CC) $(CFLAGS) -o clientTCP clientTCP.c

getip: getip.c
	$(CC) $(CFLAGS) -o getip getip.c


clean:
	rm download getip clientTCP