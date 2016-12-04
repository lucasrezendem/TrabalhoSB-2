CC = gcc
CFLAGS = -Wall -ansi
DEPS = parser.h realocador.h
OBJ = parser.o realocador.o
OBJ += probe.o
SIM = simulador.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

probe: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

simulador: $(SIM)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f parser.o
	rm -f probe.o
	rm -f realocador.o
	rm -f probe
