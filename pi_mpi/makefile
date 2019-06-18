CC = mpicc
INC = -I./inc

all: main

main: folder pi.o
	$(CC) obj/pi.o -o app.bin

pi.o: 
	$(CC) -o pi.c -c $(INC) src/pi.c -o obj/pi.o

folder: 
	mkdir -p obj

clean:
	rm -rf obj app.bin