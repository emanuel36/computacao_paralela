INCLUDEDIR=./include
LIBDIR=./lib
BINDIR=./bin
SRCDIR=./src

all: main

matriz: $(SRCDIR)/matriz.c $(INCLUDEDIR)/matriz.h
	gcc -I$(INCLUDEDIR) -c $(SRCDIR)/matriz.c -fPIC -o $(BINDIR)/matriz.o
	gcc -shared $(BINDIR)/matriz.o -o $(LIBDIR)/libmatriz.so

main: matriz
	gcc -I$(INCLUDEDIR) -c $(SRCDIR)/main.c -o $(BINDIR)/main.o
	gcc $(BINDIR)/main.o -L$(LIBDIR) -lmatriz -o $(BINDIR)/matmultvetor
	gcc $(SRCDIR)/comparaMatrizes.c -o $(BINDIR)/comparaMatrizes
	gcc $(SRCDIR)/criarMatriz.c -o $(BINDIR)/criarMatriz

clean:
	rm $(LIBDIR)/*.so
	rm $(BINDIR)/*
