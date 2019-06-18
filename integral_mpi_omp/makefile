CC = mpicc
INC = -I./inc

all: paralelo serial

serial: folder serial.o
	$(CC) obj/serial.o -o serial.bin -lm

paralelo: folder paralelo.o
	$(CC) -fopenmp obj/paralelo.o -o paralelo.bin -lm

paralelo.o: 
	$(CC) -fopenmp -o paralelo.c -c $(INC) src/paralelo.c -o obj/paralelo.o

serial.o: 
	$(CC) -o serial.c -c $(INC) src/serial.c -o obj/serial.o

folder: 
	mkdir -p obj

clean:
	rm -rf obj *.bin