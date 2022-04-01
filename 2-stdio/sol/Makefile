CC = gcc
CFLAGS = -Wall -fPIC

LDFLAGS = -L.

build: libso_stdio.so

libso_stdio.so: libso_stdio.o
	$(CC) -shared -o $@ $^

libso_stdio.o: libso_stdio.c

clean:
	 rm -f *.o   libso_stdio.so

.PHONY: all clean
