CC = gcc
CFLAGS = -Wall -ansi
DEPS = passagens.h pre_mcr.h
OBJ = passagens.o pre_mcr.o montador.o
SIM = simulador.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

montador: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

simulador: $(SIM)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f passagens.o
	rm -f pre_mcr.o
	rm -f montador.o
	rm -f simulador.o
