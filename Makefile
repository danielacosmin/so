CC = gcc 
CFLAGS = -Wall -fPIC
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.

build:
	$(CC) $(CFLAGS) -c so_stdio.c 
	$(CC) -shared so_stdio.o -o libso_stdio.so
clean:
	rm -f so_stdio.o libso_stdio.so
