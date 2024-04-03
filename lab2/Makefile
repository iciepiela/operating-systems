CC=gcc
CFLAGS=-Wall -g


collatz_static: collatz.c
	$(CC) $(CFLAGS) -c collatz.c -o collatz.o
	ar rcs libcollatz.a collatz.o

collatz_shared: collatz.c
	$(CC) -fPIC -c collatz.c -o collatz.o
	$(CC) $(CFLAGS) -shared -o libcollatz.so collatz.o



client_static: client.c
	make collatz_static
	$(CC) $(CFLAGS) -o client client.c libcollatz.a

client_shared: client.c
	make collatz_shared
	$(CC) $(CFLAGS) client.c -o client -L./ -lcollatz -Wl,-rpath,. 

client_dynamic: client.c
	make collatz_shared
	$(CC) $(CFLAGS) client.c -o client -ldl -D DYNAMIC_LINK
 
clean:
	rm -f *.o *.a *.so*
	rm -f client
