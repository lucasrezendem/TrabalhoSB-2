#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pre_mcr.h"
#include "passagens.h"

int main(int argc, char *argv[]){
	char *aux;
	int i;

	if(argc > 4){
		printf("ERRO: ARGUMENTOS EM EXCESSO! INSIRA SOMENTE DE UM A TRÊS ARQUIVOS A SEREM MONTADOS.\n");
		return -1;
	}
	else if(argc < 2){
		printf("ERRO: FALTAM ARGUMENTOS! INSIRA DE UM A TRÊS ARQUIVOS A SEREM MONTADOS.\n");
		return -1;
	}

	for(i=1; i<argc; i++){
    	aux = preprocessamento(argv[i], NULL);
    	/*aux = expansaoDeMacros(aux, NULL); */
    	duasPassagens(aux, argc-1, argv[i]);
    }

	return 0;
}
