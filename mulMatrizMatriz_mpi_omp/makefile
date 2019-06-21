CC = mpicc
INC = -I./inc

all: main

main: folder matriz.o main.o
	$(CC) -fopenmp obj/matriz.o obj/main.o -o app.bin

main.o: 
	$(CC) -fopenmp -o main.c -c $(INC) src/main.c -o obj/main.o

matriz.o: 
	$(CC) -o matriz.c -c $(INC) src/matriz.c -o obj/matriz.o

folder: 
	mkdir -p obj

clean:
	rm -rf obj app.bin resultado.txt