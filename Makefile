CC = gcc
CFLAGS = -Wall -ansi
DEPS = parser.h realocador.h
OBJ = parser.o realocador.o
OBJ += carregador.o
SIM = simulador.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

carregador: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

simulador: $(SIM)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f parser.o
	rm -f carregador.o
	rm -f realocador.o
	rm -f carregador
