CC = gcc
CFLAGS = -Wall -ansi
DEPS = parser.h realocador.h simulador.h
OBJ = parser.o realocador.o simulador.o
OBJ += carregador.o
SIM = simulador.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

carregador: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f parser.o
	rm -f carregador.o
	rm -f realocador.o
	rm -f simulador.o
	rm -f carregador
	rm -f PROGA.im
