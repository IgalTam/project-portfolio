CC = gcc

CFLAGS = -g -Wall

mush2: mush2.o mush2Util.o mush2uu.o
	$(CC) $(CFLAGS) -o mush2 mush2.o mush2Util.o mush2uu.o -L ~pn-cs357/Given/Mush/libmush/lib64 -lmush

mush22: mush2.o mush2Exp.o mush2uu2.o
	$(CC) $(CFLAGS) -o mush22 mush2.o mush2Exp.o mush2uu2.o -L ~pn-cs357/Given/Mush/libmush/lib64 -lmush

mush2.o: mush2.c
	$(CC) -c $(CFLAGS) -I ~pn-cs357/Given/Mush/libmush/include -o mush2.o mush2.c

mush2Util.o: mush2Util.c
	$(CC) -c $(CFLAGS) -I ~pn-cs357/Given/Mush/libmush/include -o mush2Util.o mush2Util.c

mush2uu.o: mush2uu.c
	$(CC) -c $(CFLAGS) -I ~pn-cs357/Given/Mush/libmush/include -o mush2uu.o mush2uu.c

mush2Exp.o: mush2Exp.c
	$(CC) -c $(CFLAGS) -I ~pn-cs357/Given/Mush/libmush/include -o mush2Exp.o mush2Exp.c

mush2uu2.o: mush2uu2.c
	$(CC) -c $(CFLAGS) -I ~pn-cs357/Given/Mush/libmush/include -o mush2uu2.o mush2uu2.c


clean:
	-rm -f *.o
