CC=gcc
CFLAGS=-Wall -g

Objects = zad1 calc menager

all: $(Objects)

zad1: zad1.c
	$(CC) $(CFLAGS) -o zad1 zad1.c

calc: calc.c
	$(CC) $(CFLAGS) -o calc calc.c
	
menager: menager.c
	$(CC) $(CFLAGS) -o menager menager.c




clean:
	rm -f $(Objects)