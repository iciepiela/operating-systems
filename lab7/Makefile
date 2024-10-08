CC = gcc
FLAGS = -Wall -g


.PHONY: client server clean 

Objects = client server

all: $(Objects)

client: client.c
	$(CC) $(FLAGS) client.c -o client

server: server.c
	$(CC) $(FLAGS) server.c -o server

clean:
	rm -f $(Objects)
