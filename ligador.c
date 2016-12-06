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


typedef struct mapaBits { /*estrutura de dados para armazenar informações do mapa de bits*/
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
				if (aux==0 || (strcmp ("TU:",label)==0 && situ == INTERNO))
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

	while(!feof(fp)){
		fscanf(fp,"%s",H);
		if(strcmp(H,"H:")==0){
			cont++;
		}
		if(cont == 2){
			fscanf(fp,"%c", &c);
			while(c!='\n'){
				fscanf(fp,"%d", &corretor);
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
	if(lis!=NULL){
		while(aux!=NULL){
			if(aux->externo == INTERNO){
				aux->valor += fator;

			}
			aux = aux->next;
		}
	}
}
tabSimb *tabGlobal (tabSimb *lis1, tabSimb *lis2, tabSimb *lis3){
	tabSimb *orig = lis1, *aux2 = lis2, *aux3 = lis3;
	while(orig != NULL){
		aux2 = lis2;
		aux3 = lis3;
		if(orig->externo == EXTERNO) {

			if(lis2!=NULL){
				while(aux2 != NULL){
					if (strcmp(orig->simbolo, aux2->simbolo)==0 && aux2->externo == INTERNO)
						orig->valor += aux2->valor;
					aux2 = aux2->next;
				}
			}
			if(lis3!=NULL){
				while(aux3 != NULL){
					if (strcmp(orig->simbolo, aux3->simbolo)==0 && aux3->externo == INTERNO)
						orig->valor += aux3->valor;
					aux3 = aux3->next;
				}
			}
		}

		orig = orig->next;
	}
return lis1;
}

void escreveMapaArq (char *nome, int fator, mapaBits *A, mapaBits *B, mapaBits *C){
	mapaBits *aux1 = A, *aux2 = B, *aux3 = C;
	char nomeAux[TAM];
	int tamanho = strlen (nome);
	strcpy(nomeAux, nome);
	nomeAux[tamanho-2]='\0';

	FILE *fp = fopen (nomeAux, "w+");
	if(!fp){
		printf("Não foi possível criar arquivo");
		exit(1);
	}
	fprintf(fp,"H: %s\nH: %d\nH: ",nomeAux, fator);

	while(aux1!=NULL){
		fprintf(fp,"%d",aux1->valor);
		aux1 = aux1->next;
	}
	if(B!=NULL){
		while(aux2!=NULL){
			fprintf(fp,"%d",aux2->valor);
			aux2 = aux2->next;
		}
	}
	if(C!=NULL){
		while(aux3!=NULL){
			fprintf(fp,"%d",aux3->valor);
			aux3 = aux3->next;
		}
	}
	fprintf(fp,"\nT: ");
	fclose(fp);
}

int achaT (FILE *fp){
	char c;
	int cont=0;
	fseek(fp,0,SEEK_SET);
	while(!feof(fp)){
		c = fgetc(fp);
		if(c=='\n')
			cont++;
		if(cont==5){
			fseek(fp,3,SEEK_CUR);
			return ftell(fp);
		}

	}
return ftell(fp);
}

void escreveSaida (FILE *fp_orig, char *nome, tabSimb *tabS, tabSimb *tabU, mapaBits *mpBits, int fator){
	mapaBits *bits = mpBits;
	tabSimb *simb = tabS, *tabUso = tabU;
	int valor, impr=0, pos=0, achou=0;
	char xx[2],c;
	char nomeAux[TAM];
	int tamanho = strlen (nome);
	strcpy(nomeAux, nome);
	nomeAux[tamanho-2]='\0';

	FILE *fp_sai = fopen (nomeAux, "a+");
	if(!fp_sai){
		printf("Não foi possível criar arquivo");
		exit(1);
	}

	valor = achaT (fp_orig);
	fseek(fp_orig, valor, SEEK_SET);
	while(!feof(fp_orig)){
		fscanf(fp_orig,"%s", xx);
		if(xx[0]!='x'){
			impr = atoi(xx);
			if(bits->valor == 0){
				fprintf(fp_sai,"%d ",impr);
			}
			else{
				tabUso = tabU;
				achou = 0;
				while(tabUso !=NULL){
					if(tabUso->valor-fator == pos){
						simb = tabS;
						while(simb!=NULL){
							if(strcmp(simb->simbolo,tabUso->simbolo)==0){
								fprintf(fp_sai,"%d ",(impr+simb->valor));
								achou=1;
							}

							simb = simb->next;
						}

					}
					tabUso = tabUso -> next;
				}
				if(achou==0){
					fprintf(fp_sai,"%d ",(impr+fator));
				}
			}
			if(bits->next!=NULL)
				bits = bits->next;
			pos++;

		}
		else{
			fprintf(fp_sai,"%s ",xx);
		}
		c = fgetc(fp_orig);
		c = fgetc(fp_orig);
		if(c!=EOF)
			fseek(fp_orig,-2,SEEK_CUR);
	}
	fclose(fp_sai);
}



void ligador (char *arq1, char *arq2, char *arq3){
	int caso = 0, fator1=0, fator2=0, fator3=0;
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

	fseek(fp1,0,SEEK_SET);
	tabSimb *tabUso1 = pegaTDTU (NULL,fp1,"TU:",INTERNO);
	tabSimb *tabUso2 = NULL, *tabUso3 = NULL;

	if (caso>N_ARQU1){
		(fp2) = fopen (arq2, "r");
		if(!fp2){
			printf("Erro ao abrir arquivo");
		}
		sequencia2 = pegaMapaBits (fp2);
		simbolos2 = pegaTDTU(NULL, fp2, "TD:", INTERNO);
		simbolos2 = pegaTDTU(simbolos2, fp2, "TU:", EXTERNO);
		fseek(fp2,0,SEEK_SET);
		tabUso2 = pegaTDTU (NULL,fp2,"TU:",INTERNO);
		if (caso==N_ARQU3){
			(fp3) = fopen (arq3, "r");
			if(!fp3){
				printf("Erro ao abrir arquivo");
			}
			sequencia3 = pegaMapaBits (fp3);
			simbolos3 = pegaTDTU(NULL, fp3, "TD:", INTERNO);
			simbolos3 = pegaTDTU(simbolos3, fp3, "TU:", EXTERNO);
			fseek(fp3,0,SEEK_SET);
			tabUso3 = pegaTDTU (NULL,fp3,"TU:",INTERNO);
		}
	}
	/* PASSA FATOR DE CORREÇÃO NAS TABELAS DE SIMBOLOS DO PROGRAMAS B E C*/
	if(caso>=N_ARQU1){
		fseek(fp1,0,SEEK_SET);
		fator1 = pegaFatorCorr (fp1);
		corrige (simbolos2, fator1);
		corrige (tabUso2, fator1);
	}
	if(caso>=N_ARQU2){
		fseek(fp2,0,SEEK_SET);
		fator2 = pegaFatorCorr (fp2);
		fator2 += fator1;
		if(caso==N_ARQU3){
			corrige (simbolos3, fator2);
			corrige (tabUso3, fator1);
			fator3 = pegaFatorCorr (fp3);
		}
	}
	/* CORRIGE VALORES DA TABELA DE SIMBOLOS PARA OS CORRETOS*/
		simbolos1 = tabGlobal (simbolos1, simbolos2, simbolos3);
	if(caso>N_ARQU1){
		simbolos2 = tabGlobal (simbolos2, simbolos1, simbolos3);
	}
	if(caso == N_ARQU3){
		simbolos3 = tabGlobal (simbolos3, simbolos2, simbolos1);
	}
	/*COMEÇA A ESCREVER ARQUIVO DE SAIDA .O*/
	if(caso==N_ARQU3)
		escreveMapaArq (arq1, (fator2+fator3) , sequencia1, sequencia2, sequencia3);
	if(caso==N_ARQU2)
		escreveMapaArq (arq1, (fator2) , sequencia1, sequencia2, sequencia3);
	if(caso==N_ARQU1)
		escreveMapaArq (arq1, (fator1) , sequencia1, sequencia2, sequencia3);

	escreveSaida (fp1, arq1, simbolos1, tabUso1, sequencia1, 0);

	if(caso>N_ARQU1)
	escreveSaida (fp2, arq1, simbolos2, tabUso2, sequencia2, fator1);

	if(caso==N_ARQU3)
	escreveSaida (fp3, arq1, simbolos3, tabUso3, sequencia3, fator2);
	/*CONCLUSOES FINAIS*/
	fclose (fp1);
	liberaMapaBits (sequencia1);
	liberaTabSimb (simbolos1);
	liberaTabSimb (tabUso1);
	if(caso>N_ARQU1){
		liberaMapaBits (sequencia2);
		liberaTabSimb (simbolos2);
		liberaTabSimb (tabUso2);
		fclose(fp2);
	}
	if(caso==N_ARQU3){
		liberaMapaBits (sequencia3);
		liberaTabSimb (simbolos3);
		liberaTabSimb (tabUso3);
		fclose(fp3);
	}
}

int main (int argc, char *argv[]) {

	if (argc == 2)
		ligador (argv[1],"","");
	if (argc == 3)
		ligador (argv[1],argv[2],"");
	if (argc == 4)
		ligador (argv[1],argv[2],argv[3]);
return 0;
}
