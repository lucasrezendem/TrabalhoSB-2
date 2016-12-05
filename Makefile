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
	rm -f parser.o
	rm -f carregador.o
	rm -f realocador.o
	rm -f simulador.o
	rm -f pre_mcr.o
	rm -f passagens.o
	rm -f montador.o
	rm -f ligador.o
	rm -f montador
	rm -f carregador
	rm -f ligador
	rm -f PROGA.im
