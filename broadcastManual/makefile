CC = mpicc
INC = -I./inc

all: main

main: folder main.o
	$(CC) obj/main.o -o app.bin

main.o: 
	$(CC) -o main.c -c $(INC) src/main.c -o obj/main.o

folder: 
	mkdir -p obj

clean:
	rm -rf obj app.bin