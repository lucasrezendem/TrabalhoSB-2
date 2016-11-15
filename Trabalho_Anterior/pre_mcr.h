#ifndef PRE_MCR_H
#define PRE_MCR_H

typedef struct listMcr { /*estrutura de dados para armazenar informações de nome das macros*/
  char orig[50], eq[50];
  struct listMcr *next;

} listMcr;

typedef struct listAux { /*estrutura que armazena parametros definidos com equ*/
  int valor;
  char nome[50];
  struct listAux *next;

} list;

/*Operacoes com a listMcr*/
void liberaMcr (listMcr *aux1);
listMcr *addMcr (listMcr *l, char label[50]);
listMcr *updateMcr (listMcr *l, char label[50]);

/*Operacoes com a list*/
void libera (list* aux1);
list *add (list* l, int val, char label[50]);

/*Funcoes auxiliares a outras*/
int checaIf (list *l, char *nome);
void limpa_linhas(char *nome_arquivo);

/*Funcoes relativas ao preprocessamento*/
char* preprocessamento(char *nomeArquivoIN, char *nomeArquivoOUT);
void tiraComentario (char *nome);
void colocaLinha (char *nome);
void secaoDir (char *nome);
void leAsm (char *nome,char *nomeExt);
void arrumaTopoFim (char *nome);

/*Funcoes relativas a expansao de macros*/
char* expansaoDeMacros(char *nomeArquivoIN, char *nomeArquivoOUT);
void checaAntes (char *nome);
void lePre (char *nome,char *nomeExt);
int troca (FILE *fp,int val, char c,char aux[50], listMcr *lis);
int checaMacro(char *func, char *nome, char *nome2, int pos, int pospre);

#endif
