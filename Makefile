CC = clang

inventory: invfunc.o inventory.o
	${CC} $^ -o $@ -lsqlite3 -lsodium

invfunc.o: invfunc.c
	${CC} -c invfunc.c -o invfunc.o

inventory.o: inventory.c 
	${CC} -c inventory.c -o inventory.o

clean:
	rm -f *.o inventory