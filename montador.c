#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pre_mcr.h"
#include "passagens.h"

int main(int argc, char *argv[]){
	char *aux;

	if(argc > 4){
		printf("ERRO: ARGUMENTOS EM EXCESSO! DIGA SOMENTE O TIPO DE OPERACAO, O ARQUIVO DE ENTRADA E O ARQUIVO DE SAIDA.\n");
		return -1;
	}
	else if(argc < 4){
		printf("ERRO: FALTAM ARGUMENTOS! DIGA O TIPO DE OPERACAO, O ARQUIVO DE ENTRADA E O ARQUIVO DE SAIDA.\n");
		return -1;
	}


	if(!strcmp(argv[1], "-p")){ /*Operacao de preprocessamento selecionada*/
		aux = preprocessamento(argv[2], argv[3]);
	}
	else if(!strcmp(argv[1], "-m")){ /*Operacao de expansao de macro selecionada*/
		aux = preprocessamento(argv[2], NULL);
		aux = expansaoDeMacros(aux, argv[3]);
	}
	else if(!strcmp(argv[1], "-o")){ /*Operacao de montagem completa selecionada*/
		aux = preprocessamento(argv[2], NULL);
		aux = expansaoDeMacros(aux, NULL);
		duasPassagens(aux, argv[3]);
	}
	else{
		printf("ERRO: TIPO DE OPERACAO INVALIDO!\n");
		return -1;
	}

	return 0;
}
