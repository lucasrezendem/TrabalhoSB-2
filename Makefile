CC = gcc
CFLAGS = -Wall -ansi
DEPS = parser.h realocador.h simulador.h pre_mcr.h passagens.h
MONT = pre_mcr.o passagens.o montador.o
CARR = parser.o realocador.o simulador.o
CARR += carregador.o
LIG =  ligador.o
SIM = simulador.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all:
	make ligador
	make montador
	make carregador

ligador: $(LIG)
	$(CC) -o $@ $^ $(CFLAGS)

montador: $(MONT)
	$(CC) -o $@ $^ $(CFLAGS)

carregador: $(CARR)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.pre
	rm -f *.o
	rm -f montador
	rm -f carregador
	rm -f ligador
	rm -f *.im
