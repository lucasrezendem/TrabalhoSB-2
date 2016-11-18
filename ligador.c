#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*quantidade de arquivos fornecidos para o ligador*/
#define N_ARQU1 1
#define N_ARQU2 2
#define N_ARQU3 3
/*se simbolo é externo ou interno*/
#define INTERNO 1
#define EXTERNO 0
/* tamanho maximo do simbolo*/
#define TAM 50

typedef struct tabSimb { /*estrutura de dados para armazenar informações da tabela de simbolos*/
	char simbolo[TAM];
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
	printf("\n%s - %d - %d", aux2->simbolo, aux2->valor, aux2->externo);
       free(aux2);
    }

}


int checaTabSimb (tabSimb *aux1, char *label){
tabSimb *aux2 = aux1;

   while (aux1 != NULL)
    {
       aux2 = aux1;
       aux1 = aux1->next;
	if(strcmp(aux2->simbolo,label)==0)
		return 1;
    }
return 0;
}

tabSimb *addTabSimb (tabSimb *l, char label[TAM], int numero,int definido){
	tabSimb *novo = (tabSimb *)malloc(sizeof(tabSimb)), *aux = l;

	if(novo == NULL) exit(0);
	strcpy(novo->simbolo,label);
	novo->valor = numero;
	novo->externo = definido;
	    
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
	char H[TAM], c;
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
			break;
		}
	}
return sequencia;
}


tabSimb *pegaTDTU (tabSimb *lista, FILE *fp, char *label, int situ){
	char T[TAM],c='\0', parametro[TAM];
	int definicao=0, aux;

	while(!feof(fp)){
		fscanf(fp,"%s",T);
		if(strcmp(T,label)==0){
			fscanf(fp,"%c", &c);
			while(c!='\n'){
				fscanf(fp,"%s", parametro);
				fscanf(fp,"%d", &definicao);
				c = fgetc(fp);
				fseek(fp,-1,SEEK_CUR);
				if(situ == EXTERNO)
					definicao = 0;
				aux = checaTabSimb(lista, parametro);
				if (aux==0)
					lista = addTabSimb (lista,  parametro, definicao, situ);
			}
			break;
		}
	}
return lista;
}

int pegaFatorCorr (FILE *fp){
	char H[TAM], c;
	int corretor, cont=0;
	mapaBits *sequencia = NULL;

	while(!feof(fp)){
		fscanf(fp,"%s",H);
		if(strcmp(H,"H:")==0){
			cont++;
		}
		if(cont == 2){
			fscanf(fp,"%c", &c);
			while(c!='\n'){
				fscanf(fp,"%d", &corretor);
				printf("%d", corretor );
				return corretor;
			}
			cont = 0; 
			break;
		}
	}
return corretor;
}

void corrige (tabSimb *lis, int fator){
	tabSimb *aux = lis;
	while(aux->next!=NULL){
		if(aux->externo == INTERNO){
			printf("\n%s - %d - %d \n", aux->simbolo, aux-> valor, aux->externo);
			aux->valor += fator;
			printf("\n%s - %d - %d \n", aux->simbolo, aux-> valor, aux->externo);
			
		}
		aux = aux->next;
	}
}


int qntosArq (char *arq1, char *arq2, char *arq3){
	int caso = 0, corretor=0, fator=0;
	if (strcmp(arq2,"")==0){	/*checa quantos arquivos foram fornecidos ao ligador*/
		caso = N_ARQU1;
	}
	else if (strcmp(arq3,"")==0){
		caso = N_ARQU2;
	}
	else
		caso = N_ARQU3;
	/* ATÉ AQUI DEFINE QUANTOS ARQUIVOS SÃO*/
	/* ABRINDO ARQUIVOS E PEGANDO MAPA DE BITS, TD e TU DELES*/

	FILE *fp1 = fopen (arq1, "r");
	FILE *fp2, *fp3;
	if(!fp1){
		printf("Erro ao abrir arquivo");
	}
	mapaBits *sequencia1 = pegaMapaBits (fp1);
	mapaBits *sequencia2 = NULL, *sequencia3 = NULL;

	tabSimb *simbolos1 = pegaTDTU(NULL, fp1, "TD:", INTERNO);
	simbolos1 = pegaTDTU(simbolos1, fp1, "TU:", EXTERNO);

	tabSimb *simbolos2 = NULL, *simbolos3 = NULL;
	if (caso>N_ARQU1){
		(fp2) = fopen (arq2, "r");
		if(!fp2){
			printf("Erro ao abrir arquivo");
		}
		sequencia2 = pegaMapaBits (fp2);
		simbolos2 = pegaTDTU(NULL, fp2, "TD:", INTERNO);
		simbolos2 = pegaTDTU(simbolos2, fp2, "TU:", EXTERNO);
		if (caso==N_ARQU3){
			(fp3) = fopen (arq3, "r");
			if(!fp3){
				printf("Erro ao abrir arquivo");
			}
			sequencia3 = pegaMapaBits (fp3);
			simbolos3 = pegaTDTU(NULL, fp3, "TD:", INTERNO);
			simbolos3 = pegaTDTU(simbolos3, fp3, "TU:", EXTERNO);
		}
	}
	/* PASSA FATOR DE CORREÇÃO NAS LISTAS DO PROGRAMAS B E C*/
	fseek(fp1,0,SEEK_SET);	
	fator = pegaFatorCorr (fp1);
	corrige (simbolos2, fator);

	fseek(fp2,0,SEEK_SET);	
	fator += pegaFatorCorr (fp2);
	corrige (simbolos3, fator);

	/*CONCLUSOES FINAIS*/
	fclose (fp1);
	liberaMapaBits (sequencia1);
	liberaTabSimb (simbolos1);
	
	if(caso>N_ARQU1){
		liberaMapaBits (sequencia2);
		liberaTabSimb (simbolos2);
		fclose(fp2);
	}
	if(caso==N_ARQU3){
		liberaMapaBits (sequencia3);
		liberaTabSimb (simbolos3);
		fclose(fp3);
	}
	return caso;
}

int main () {
	printf("\ncomeca\n");
	int result = qntosArq ("PROGA.o","PROGB.o","PROGC.o");
	printf("\nfim\n");
return 0;
}
