#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define N_ARQU1 1 /*quantidade de arquivos fornecidos para o ligador*/
#define N_ARQU2 2
#define N_ARQU3 3

typedef struct tabSimb { /*estrutura de dados para armazenar informações da tabela de simbolos*/
	char simbolo[50];
	int valor;
	int externo;
	struct tabSimb *next;
} tabSimb;


void liberaTabSimb (tabSimb *aux1){
tabSimb *aux2 = aux1;

   while (aux1 != NULL)
    {
       aux2 = aux1;
       aux1 = aux1->next;
       free(aux2);
    }

}

tabSimb *addTabSimb (tabSimb *l, char label[50], int numero,int mapa){
	tabSimb *novo = (tabSimb *)malloc(sizeof(tabSimb)), *aux = l;

	if(novo == NULL) exit(0);
	strcpy(novo->simbolo,label);
	novo->valor = numero;
	novo->externo = mapa;
	    
	if(aux == NULL){
		novo->next = NULL;
		 return novo; 
	}
	while(aux->next !=NULL){
		aux=aux->next;
	}
	aux->next = novo;
	return l;
}

typedef struct mapaBits { /*estrutura de dados para armazenar informações da tabela de simbolos*/
	int valor;
	struct mapaBits *next;
} mapaBits;


void liberaMapaBits (mapaBits *aux1){
mapaBits *aux2 = aux1;

   while (aux1 != NULL)
    {
       aux2 = aux1;
       aux1 = aux1->next;
       free(aux2);
    }

}

mapaBits *addMapaBits (mapaBits *l, int numero){
	mapaBits *novo = (mapaBits *)malloc(sizeof(mapaBits)), *aux = l;
	novo -> valor = numero;
        if (l == NULL)
		l = novo;
        else{
		while(aux->next!=NULL)
			aux = aux->next;
		aux->next = novo;
	}
return l;
}

mapaBits *pegaMapaBits (FILE *fp){
	char H[2], c;
	int zeroOuUm=0, cont=0;
	mapaBits *sequencia = NULL;

	while(!feof(fp)){
		fscanf(fp,"%s",H);
		if(strcmp(H,"H:")==0){
			cont++;
		}
		if(cont == 3){
			fscanf(fp,"%c", &c);
			while(c!='\n'){
				fscanf(fp,"%c", &c);
				zeroOuUm = c-'0';
				if(c!='\n')
					sequencia = addMapaBits (sequencia, zeroOuUm);
			}
			cont = 0; 
		}
	}
	return sequencia;
}


int qntosArq (char *arq1, char *arq2, char *arq3){
	int caso = 0;
	if (strcmp(arq2,"")==0){	/*checa quantos arquivos foram fornecidos ao ligador*/
		caso = N_ARQU1;
	}
	else if (strcmp(arq3,"")==0){
		caso = N_ARQU2;
	}
	else
		caso = N_ARQU3;
	/* ATÉ AQUI DEFINE QUANTOS ARQUIVOS SÃO*/
	/* ABRINDO ARQUIVOS E PEGANDO MAPA DE BITS DELES*/

	FILE *fp1 = fopen (arq1, "r");
	FILE *fp2, *fp3;
	if(!fp1){
		printf("Erro ao abrir arquivo");
	}
	mapaBits *sequencia1 = pegaMapaBits (fp1);
	mapaBits *sequencia2 = NULL, *sequencia3 = NULL;
	if (caso>N_ARQU1){
		(fp2) = fopen (arq2, "r");
		if(!fp2){
			printf("Erro ao abrir arquivo");
		}
		sequencia2 = pegaMapaBits (fp2);
		if (caso==N_ARQU3){
			(fp3) = fopen (arq3, "r");
			if(!fp3){
				printf("Erro ao abrir arquivo");
			}
			sequencia3 = pegaMapaBits (fp3);
		}
	}
	
	/*CONCLUSOES FINAIS*/
	fclose (fp1);
	liberaMapaBits (sequencia1);
	if(caso>N_ARQU1){
		liberaMapaBits (sequencia2);
		fclose(fp2);
	}
	if(caso==N_ARQU3){
		liberaMapaBits (sequencia3);
		fclose(fp3);
	}
	return caso;
}

int main () {

	int result = qntosArq ("PROGA.o","PROGB.o","PROGC.o");
return 0;
}
