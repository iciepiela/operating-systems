CC=gcc
CFLAGS=-Wall -g

Objects = printers users

all: $(Objects)

printers: printers.c
	$(CC) $(CFLAGS) -o printers printers.c

users: users.c
	$(CC) $(CFLAGS) -o users users.c
	

clean:
	rm -f $(Objects)