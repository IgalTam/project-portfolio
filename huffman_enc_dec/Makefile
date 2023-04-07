CC = gcc 

CFLAGS = -g -Wall

hencode: hencode.o huffTree.o
	$(CC) $(CFLAGS) -o hencode hencode.o huffTree.o

hdecode: hdecode.o huffTree.o
	$(CC) $(CFLAGS) -o hdecode hdecode.o huffTree.o

all: hencode hdecode

hencode.o: hencode.c
	$(CC) -c $(CFLAGS) -o hencode.o hencode.c

hdecode.o: hdecode.c
	$(CC) -c $(CFLAGS) -o hdecode.o hdecode.c

huffTree.o: huffTree.c
	$(CC) -c $(CFLAGS) -o huffTree.o huffTree.c

clean:
	-rm -f *.o
