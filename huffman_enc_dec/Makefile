CC = gcc 

CFLAGS = -g -Wall

hencode: hencode.o huffTree.o
	$(CC) $(CFLAGS) -o hencode hencode.o huffTree.o

hdecode: hdecode.o huffTree.o
	$(CC) $(CFLAGS) -o hdecode hdecode.o huffTree.o

all: hencode hdecode

testEn: hencode.o hdecode.o
	./hencode test u.out
	~pn-cs357/demos/hencode test ref.out
	od -tx1 u.out > u.x
	od -tx1 ref.out > ref.x
	diff u.x ref.x
	./hdecode ref.out u.out
	~pn-cs357/demos/hdecode ref.out ref2.out
	diff u.out ref2.out

test2En: hencode.o hdecode.o
	./hencode test2 u.out
	~pn-cs357/demos/hencode test2 ref.out
	od -tx1 u.out > u.x
	od -tx1 ref.out > ref.x
	diff u.x ref.x
	./hdecode ref.out u.out
	~pn-cs357/demos/hdecode ref.out ref2.out
	diff u.out ref2.out

test3En: hencode.o hdecode.o
	./hencode test3 u.out
	~pn-cs357/demos/hencode test3 ref.out
	od -tx1 u.out > u.x
	od -tx1 ref.out > ref.x
	diff u.x ref.x
	./hdecode ref.out u.out
	~pn-cs357/demos/hdecode ref.out ref2.out
	diff u.out ref2.out

test4En: hencode.o hdecode.o
	./hencode test4 u.out
	~pn-cs357/demos/hencode test4 ref.out
	od -tx1 u.out > u.x
	od -tx1 ref.out > ref.x
	diff u.x ref.x
	./hdecode ref.out u.out
	~pn-cs357/demos/hdecode ref.out ref2.out
	diff u.out ref2.out

test5En: hencode.o hdecode.o
	./hencode test5 u.out
	~pn-cs357/demos/hencode test5 ref.out
	od -tx1 u.out > u.x
	od -tx1 ref.out > ref.x
	diff u.x ref.x
	./hdecode ref.out u.out
	~pn-cs357/demos/hdecode ref.out ref2.out
	diff u.out ref2.out

test6En: hencode.o hdecode.o
	./hencode test6 u.out
	~pn-cs357/demos/hencode test6 ref.out
	od -tx1 u.out > u.x
	od -tx1 ref.out > ref.x
	diff u.x ref.x
	./hdecode ref.out u.out
	~pn-cs357/demos/hdecode ref.out ref2.out
	diff u.out ref2.out

testNulEn: hencode.o hdecode.o
	./hencode testBlank u.out
	~pn-cs357/demos/hencode testBlank ref.out
	od -tx1 u.out > u.x
	od -tx1 ref.out > ref.x
	diff u.x ref.x
	./hdecode ref.out u.out
	~pn-cs357/demos/hdecode ref.out ref2.out
	diff u.out ref2.out

testOneEn: hencode.o hdecode.o
	./hencode testOne u.out
	~pn-cs357/demos/hencode testOne ref.out
	od -tx1 u.out > u.x
	od -tx1 ref.out > ref.x
	diff u.x ref.x
	./hdecode testOneBin.out u.out
	~pn-cs357/demos/hdecode testOneBin.out ref2.out
	diff u.out ref2.out

hencode.o: hencode.c
	$(CC) -c $(CFLAGS) -o hencode.o hencode.c

hdecode.o: hdecode.c
	$(CC) -c $(CFLAGS) -o hdecode.o hdecode.c

huffTree.o: huffTree.c
	$(CC) -c $(CFLAGS) -o huffTree.o huffTree.c

clean:
	-rm -f *.o
