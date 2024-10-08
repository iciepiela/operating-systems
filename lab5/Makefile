CC=gcc
CFLAGS=-Wall -g

Objects = zad1 catcher sender

all: $(Objects)

zad1: zad1.c
	$(CC) $(CFLAGS) -o zad1 zad1.c

catcher: catcher.c
	$(CC) $(CFLAGS) -o catcher catcher.c

sender: sender.c
	$(CC) $(CFLAGS) -o sender sender.c
	

clean:
	rm -f $(Objects)