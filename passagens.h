#ifndef PASSAGENS_H
#define PASSAGENS_H

typedef struct simbolo {
  char nome[50];
  int tipo;
  int posicao;
  int valor;
  int tam;
} Simbolo;

typedef struct listSimbolo ListSimbolo;

struct listSimbolo {
  Simbolo simbolo;
  ListSimbolo *prox;
};

typedef struct listDefinicoesTemp ListDefinicoesTemp;

struct listDefinicoesTemp {
  char nomeSimboloPublic[50];
  ListDefinicoesTemp *prox;
};

typedef struct listTabUso ListTabUso;

struct listTabUso {
  char nomeSimboloExtern[50];
  int posicaoDoUso;
  ListTabUso *prox;
};

typedef struct listProgramaFinal ListProgramaFinal;

struct listProgramaFinal {
  char linha[50];
  ListProgramaFinal *prox;
};

typedef struct listMapaBits ListMapaBits;

struct listMapaBits {
  char linha_de_bits[4];
  ListMapaBits *prox;
};

typedef struct instrucao {
  char nome[7];
  char opcode[3];
  int operandos;
} Instrucao;

typedef struct diretiva {
  char nome[7];
  int operandos;
  int espaco;
} Diretiva;

/*Operacoes sobre a tabela de símbolos, tabela de definicoes, tabela de uso, lista do programa final e do mapa de bits*/
void adicionaSimbolo(Simbolo sim);
void adicionaDefinicao(Simbolo sim);
void adicionaDefinicaoTemp(char rotulo[]);
void adicionaUso(char rotulo[], int posicao);
void adicionaLinhaProgFinal(char nova_linha[]);
void adicionaLinhaDeBits(char nova_linha_de_bits[]);
ListSimbolo *procuraSimbolo( char *NomeSim);
ListDefinicoesTemp *procuraDefinicaoTemp(char rotulo[]);
void removeSimbolo(Simbolo sim);
void esvaziaTabelas();
void imprimeSimbolos();

/*Operacoes sobre as tabelas de instrucoes e diretivas*/
int procuraInstrucao( char *nome, int operandos);
int procuraInstrucaoNom( char *nome);
int procuraDiretiva( char *nome, int operandos);

int separaTokens(FILE *fp, char tokens[10][50]);
void getNumLinha(char *dest,  char *token);
void validaTokens(int i, char tokens[10][50],  char *numLinha, int instPos);
void validaSecao( char tokens[10][50],  char *numLinha);
int getInstPos( char tokens[10][50],  char *numLinha, int i, int passagem);
int calculaEspaco( char tokens[10][50],  char *numLinha, int instPos, int i);
void verificaBeginEnd(char *arquivoIN);

void primeiraPassagem(FILE *fp, int NumArgs);

char* separaTokenDoOffset(char *token, int *offset);
int getErroCompilacao();
void verificaSecaoAtual( char tokens[10][50]);
void verificaEspacoAlocado(Simbolo simb, int offset,  char *numLinha);
void verificaStops();

void segundaPassagem(FILE *fp);

void monta_arquivo_final(char *nomeArquivoIN, FILE *fpOUT);
char* convert_asm_to_o(char nomeArquivo[]);
void duasPassagens(char *nomeArquivoIN, int NumArgs, char *arqOrig);

void resetInMacro();
#endif
