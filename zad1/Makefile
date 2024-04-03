CC=gcc
CFLAGS=-Wall -g

.PHONY: build_flipper run_test clean

build_flipper: flipper.c
	gcc flipper.c -o flipper

run_test: flipper
	mkdir output
	./flipper ./art ./output

clean:
	rm -f flipper
	rm -dr output