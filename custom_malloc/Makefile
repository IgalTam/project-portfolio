CC = gcc

CFLAGS = -Wall -g -fpic

intel-all: lib/libmalloc.so lib64/libmalloc.so

lib/libmalloc.so: lib malloc32.o
	$(CC) $(CFLAGS) -m32 -shared -o $@ malloc32.o
	
lib64/libmalloc.so: lib64 malloc64.o
	$(CC) $(CFLAGS) -shared -o $@ malloc64.o

lib:
	mkdir lib

lib64:
	mkdir lib64

malloc32.o: malloc.c
	$(CC) $(CFLAGS) -m32 -c -o malloc32.o malloc.c

malloc64.o: malloc.c
	$(CC) $(CFLAGS) -m64 -c -o malloc64.o malloc.c

bad_mal: bad_mal.c
	gcc -Wall -g -c -o bad_mal.o bad_mal.c
	gcc -L ./lib64 -o bad_mal bad_mal.o -lmalloc
	# also need to run in terminal: 
	# LD_LIBRARY_PATH=./lib64:$LD_LIBRARY_PATH
	# export LD_LIBRARY_PATH

bad_mal32: bad_mal.c
	gcc -Wall -g -c -o bad_mal.o bad_mal.c
	gcc -L ./lib -o bad_mal bad_mal.o -lmalloc
	# also need to run in terminal: 
	# LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH
	# export LD_LIBRARY_PATH

clean:
	-rm -f *.o
	-rm -f *.so
	-rm -rf lib
	-rm -rf lib64
	-rm -f bad_mal
