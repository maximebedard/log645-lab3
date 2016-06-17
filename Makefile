CFLAGS=-g -fPIC -m64 -Wall
LFLAGS=-fPIC -m64 -Wall
MPICC=mpicc

all: chaleur

chaleur.o: chaleur.c
	$(MPICC) $(CFLAGS) -c chaleur.c -o $@

chaleur: chaleur.o
	$(MPICC) $(LFLAGS) chaleur.o -o chaleur

ps:
	ps -fu $$USER

clean:
	rm chaleur
	rm chaleur.o
