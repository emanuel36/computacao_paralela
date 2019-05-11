BASEDIR=/home/jmhal/repositorios/parallel/serial/
INCLUDEDIR=$(BASEDIR)/include
LIBDIR=$(BASEDIR)/lib
BINDIR=./bin
SRCDIR=./src

all: main

main:
	mpicc -I$(INCLUDEDIR) -c $(SRCDIR)/main.c -o $(BINDIR)/main.o
	mpicc $(BINDIR)/main.o -L$(LIBDIR) -lmatriz -o $(BINDIR)/matmultvetor

clean:
	rm $(LIBDIR)/*.so
	rm $(BINDIR)/*
