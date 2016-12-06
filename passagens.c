#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "passagens.h"

#define NUM_INSTRUCOES 14
#define NUM_DIRETIVAS 9
#define NAO_ENCONTRADO -1
#define EXCESSO_OPERANDOS -2
#define FALTA_OPERANDOS -3
#define CONSTANTE 268 /*valores pra nao entrar em conflito com nada*/
#define VARIAVEL 269
#define LABEL 270
#define EXTERN 271
#define NUM_TOKENS 10

static const Instrucao instrucoes[NUM_INSTRUCOES] = {
                                        {"ADD", "01", 1},
                                        {"SUB", "02", 1},
                                        {"MULT", "03", 1},
                                        {"DIV", "04", 1},
                                        {"JMP", "05", 1},
                                        {"JMPN", "06", 1},
                                        {"JMPP", "07", 1},
                                        {"JMPZ", "08", 1},
                                        {"COPY", "09", 2},
                                        {"LOAD", "10", 1},
                                        {"STORE", "11", 1},
                                        {"INPUT", "12", 1},
                                        {"OUTPUT", "13", 1},
                                        {"STOP", "14", 0}
                                      };

static const Diretiva diretivas[NUM_DIRETIVAS] = {
                                      {"IF", 1, 0},
                                      {"EQU", 1, 0},
                                      {"SPACE", 0, 1},
                                      {"SPACE", 1, 1},
                                      {"CONST", 1, 1},
                                      {"BEGIN", 0, 0},
                                      {"END", 0, 0},
                                      {"PUBLIC", 1, 0}, /*embora os simbolos nao tenham o tipo explicitamente PUBLIC nesse programa, ter esse tipo nessa lista vai facilitar na segunda passagem*/
                                      {"EXTERN", 0, 0}
                                    };

static ListSimbolo *lsimbolos = NULL;
static ListSimbolo *ldefinicoes = NULL;
static ListDefinicoesTemp *ldefinicoesTemp = NULL;
static ListTabUso *luso = NULL;
static ListProgramaFinal *lprogfinal = NULL;
static ListMapaBits *lmapabits = NULL;
static int contPos = 0;
static int tamanhoPrograma = 0;
static int secText = 0;
static int quantStops = 0; /*conta quantas instrucoes "Stop" tem no programa*/
static int erroCompilacao = 0;/*flag que define se a tabela de simbolos deve ser construida*/
static int inMacro = 0;
static int achouBegin = 0;
static int achouEnd = 0;

void adicionaSimbolo(Simbolo sim) {
  ListSimbolo *aux = lsimbolos;
  if (lsimbolos == NULL) {
    lsimbolos = malloc(sizeof(ListSimbolo));
    lsimbolos->simbolo = sim;
    lsimbolos->prox = NULL;
  } else {
    while (aux->prox != NULL) aux = aux->prox;
    aux->prox = malloc(sizeof(ListSimbolo));
    aux->prox->simbolo = sim;
    aux->prox->prox = NULL;
  }
}

void adicionaDefinicao(Simbolo sim) {
  ListSimbolo *aux = ldefinicoes;
  if (ldefinicoes == NULL) {
    ldefinicoes = malloc(sizeof(ListSimbolo));
    ldefinicoes->simbolo = sim;
    ldefinicoes->prox = NULL;
  } else {
    while (aux->prox != NULL) aux = aux->prox;
    aux->prox = malloc(sizeof(ListSimbolo));
    aux->prox->simbolo = sim;
    aux->prox->prox = NULL;
  }
}

void adicionaDefinicaoTemp(char rotulo[]) {
  ListDefinicoesTemp *aux = ldefinicoesTemp;
  if (ldefinicoesTemp == NULL) {
    ldefinicoesTemp = malloc(sizeof(ListDefinicoesTemp));
    strcpy(ldefinicoesTemp->nomeSimboloPublic, rotulo);
    ldefinicoesTemp->prox = NULL;
  } else {
    while (aux->prox != NULL) aux = aux->prox;
    aux->prox = malloc(sizeof(ListDefinicoesTemp));
    strcpy(aux->prox->nomeSimboloPublic, rotulo);
    aux->prox->prox = NULL;
  }
}

void adicionaUso(char rotulo[], int posicao) {
  ListTabUso *aux = luso;
  if (luso == NULL) {
    luso = malloc(sizeof(ListTabUso));
    strcpy(luso->nomeSimboloExtern, rotulo);
    luso->posicaoDoUso = posicao;
    luso->prox = NULL;
  } else {
    while (aux->prox != NULL) aux = aux->prox;
    aux->prox = malloc(sizeof(ListTabUso));
    strcpy(aux->prox->nomeSimboloExtern, rotulo);
    aux->prox->posicaoDoUso = posicao;
    aux->prox->prox = NULL;
  }
}

void adicionaLinhaProgFinal(char nova_linha[]) {
  ListProgramaFinal *aux = lprogfinal;
  if (lprogfinal == NULL) {
    lprogfinal = malloc(sizeof(ListProgramaFinal));
    strcpy(lprogfinal->linha, nova_linha);
    lprogfinal->prox = NULL;
  } else {
    while (aux->prox != NULL) aux = aux->prox;
    aux->prox = malloc(sizeof(ListProgramaFinal));
    strcpy(aux->prox->linha, nova_linha);
    aux->prox->prox = NULL;
  }
}

void adicionaLinhaDeBits(char nova_linha_de_bits[]) {
  ListMapaBits *aux = lmapabits;
  if (lmapabits == NULL) {
    lmapabits = malloc(sizeof(ListMapaBits));
    strcpy(lmapabits->linha_de_bits, nova_linha_de_bits);
    lmapabits->prox = NULL;
  } else {
    while (aux->prox != NULL) aux = aux->prox;
    aux->prox = malloc(sizeof(ListMapaBits));
    strcpy(aux->prox->linha_de_bits, nova_linha_de_bits);
    aux->prox->prox = NULL;
  }
}

ListSimbolo *procuraSimbolo( char *nomeSim) {
  ListSimbolo *aux = lsimbolos;
  while(aux != NULL && strcasecmp(nomeSim, aux->simbolo.nome) != 0) {
    aux = aux->prox;
  }
  return aux;
}

ListDefinicoesTemp *procuraDefinicaoTemp(char rotulo[]) {
  ListDefinicoesTemp *aux = ldefinicoesTemp;
  while(aux != NULL && strcasecmp(rotulo, aux->nomeSimboloPublic) != 0) {
    aux = aux->prox;
  }
  return aux;
}

void removeSimbolo(Simbolo sim) {
  ListSimbolo *aux = procuraSimbolo(sim.nome);
  ListSimbolo *aux2 = lsimbolos;
  if (aux == NULL) return;
  while (aux2->prox != aux) aux2 = aux2->prox;
  aux2->prox = aux->prox;
  free(aux);
}

void esvaziaTabelas() {
  ListSimbolo *aux;
  ListTabUso *aux1;
  ListDefinicoesTemp *aux2;
  ListProgramaFinal *aux3;
  ListMapaBits *aux4;
  while (lsimbolos != NULL) {
    aux = lsimbolos;
    lsimbolos = lsimbolos->prox;
    free(aux);
  }
  while (ldefinicoes != NULL) {
    aux = ldefinicoes;
    ldefinicoes = ldefinicoes->prox;
    free(aux);
  }
  while (luso != NULL) {
    aux1 = luso;
    luso = luso->prox;
    free(aux1);
  }
  while (ldefinicoesTemp != NULL) {
    aux2 = ldefinicoesTemp;
    ldefinicoesTemp = ldefinicoesTemp->prox;
    free(aux2);
  }
  while (lprogfinal != NULL) {
    aux3 = lprogfinal;
    lprogfinal = lprogfinal->prox;
    free(aux3);
  }
  while (lmapabits != NULL) {
    aux4 = lmapabits;
    lmapabits = lmapabits->prox;
    free(aux4);
  }
}

/*imprime a tabela de simbolos*/
void imprimeSimbolos() {
  ListSimbolo *aux = lsimbolos;
  if (lsimbolos == NULL) printf("Lista Vazia.\n");
  while (aux != NULL) {
    printf("%s: %d -> ", aux->simbolo.nome, aux->simbolo.posicao);
    switch (aux->simbolo.tipo) {
      case CONSTANTE:
        printf("constante -> ");
        break;
      case VARIAVEL:
        printf("variavel -> ");
        break;
      case LABEL:
        printf("label -> ");
        break;
      default:
        printf("ERRO -> ");
        break;
    }
    printf("valor: %d\n", aux->simbolo.valor);
    aux = aux->prox;
  }
}

/*retorna a posicao da instrucao no vetor ou valores de erro*/
int procuraInstrucao( char *nome, int operandos) {
  int i;
  for (i = 0; i < NUM_INSTRUCOES; i++) {
    if (strcasecmp(nome, instrucoes[i].nome) == 0){
      if (operandos > instrucoes[i].operandos) return EXCESSO_OPERANDOS;
      if (operandos < instrucoes[i].operandos) return FALTA_OPERANDOS;
      return i;
    }
  }
  return NAO_ENCONTRADO;
}

int procuraInstrucaoNom( char *nome) {
  int i;
  for (i = 0; i < NUM_INSTRUCOES; i++) {
    if (strcasecmp(nome, instrucoes[i].nome) == 0){
      return i;
    }
  }
  return NAO_ENCONTRADO;
}

/*retorna a posicao da diretiva no vetor ou valores de erro conforme o erro*/
int procuraDiretiva( char *nome, int operandos){
  int retVal = NAO_ENCONTRADO;
  int i;
  for (i = 0; i < NUM_DIRETIVAS; i++) {
    if (strcasecmp(nome, diretivas[i].nome) == 0) {
      if (operandos > diretivas[i].operandos) retVal = EXCESSO_OPERANDOS;
      else if (operandos < diretivas[i].operandos) retVal = FALTA_OPERANDOS;
      else {
        retVal = i;
        break;
        if (strcasecmp(diretivas[i].nome, "SPACE") == 0) break; /*evita que SPACE sem operandos lance erro de falta de operands*/
      }

    }
  }
  return retVal;
}

int separaTokens(FILE *fp, char tokens[10][50]) {
  int i, j;
  char linha[160] = "string_inicial", *aux = NULL; /*cada linha tem NO MAXIMO cerca de 160 caracteres*/
  fscanf(fp, "%[^\n]", linha); /*pega a linha a qual o fp estava apontando*/

  if(feof(fp) && strcmp("string_inicial", linha) == 0) {
  	return -1;
  }
  fseek(fp, 1, SEEK_CUR); /*faz o fp sair do \n e ja apontar pra proxima linha*/
  aux = strtok(linha, " ,\t");
	strcpy(tokens[0], aux);
	for(i=1; i<NUM_TOKENS; i++){
    aux = strtok(NULL, " ,\t");
    if(aux == NULL) break; /*se a linha ja tiver acabado, antes de completar os 10 elementos, acaba com o loop*/
		strcpy(tokens[i], aux);
	}
    i--;
  for(j = 2; j <= i; j++) {
    if (strlen(tokens[j]) < 3 && tokens[j][0] == '+') {
      strcat(tokens[j-1], tokens[j]); /*concatena o + ao simbolo*/

    }
    if (tokens[j-2][strlen(tokens[j-2])-1] == '+') {
      strcat(tokens[j-2], tokens[j]); /*concatena o offset a string "simbolo+"*/
      strcpy(tokens[j-1], tokens[i]); /*passa o marcador de linha pra posicao onde fica o +*/

      i -= 2; /*decrementa em 2 o maior indice dessa linha*/
    }
  }

  return i;
}

void getNumLinha(char *dest,  char *token) {
  if (strchr(token, '(')){
    strcpy(dest,token);
    memmove(dest, dest+1, strlen(dest));
  }
}

void validaTokens(int i, char tokens[10][50],  char *numLinha, int instPos) {
  int k, j, errTok = 0, digito = 0;
  for (k = 0; k < i; k++) {
    if (tokens[k] != NULL){
      if (!isalnum(tokens[k][0]) && tokens[k][0] != '_') {
        /*printf("\nERRO >> erro léxico detectado na linha: %s (Token '%s' invalido)\n", numLinha, tokens[k]);
        erroCompilacao = 1;*/
      }
      if (isdigit(tokens[k][0])) digito = 1;
      for (j = 1; j < strlen(tokens[k]); j++) {
        if (!isalnum(tokens[k][j]) && tokens[k][j] != '_') {
          if (!(j == strlen(tokens[k]) - 1 && (tokens[k][j] == ':')) && !(tokens[k][j] == '+' && isdigit(tokens[k][j+1]) && isalpha(tokens[k][j-1]))){
            errTok = 1;
            printf("Errtok\n");
          }
        }
        if (isalpha(tokens[k][j]) && digito == 1) errTok = 1;
      }
      if (errTok == 1) {
        /*printf("\nERRO >> erro léxico detectado na linha: %s (Token '%s' invalido)\n", numLinha, tokens[k]);
        erroCompilacao = 1;*/
      }
    }
  }
}

void validaSecao( char tokens[10][50],  char *numLinha) {
  /*verifica se a secao eh valida*/
  if (strcasecmp(tokens[0], "section") == 0 && !(strcasecmp(tokens[1], "data") == 0 || strcasecmp(tokens[1], "text") == 0)){
    /*erroCompilacao = 1;
    printf("\nERRO >> erro sintático detectado na linha: %s (Seção invalida)\n", numLinha);
  */}
  /*se ele achar um simbolo novo na secao de texto ele é um label*/
  else if (strcasecmp(tokens[0], "section") == 0 && strcasecmp(tokens[1], "text") == 0) secText = 1;

  /*se for na secao de dados ele é uma variavel e verifica se veio depois da section data*/
  else if (strcasecmp(tokens[0], "section") == 0 && strcasecmp(tokens[1], "data") == 0){
    if (secText == 1) secText = 0;
    else {
      erroCompilacao = 1;
     /* printf("\nERRO >> erro semântico detectado na linha: %s (Secao de dados sem secao de texto anterior)\n", numLinha);
    */}
  }
}

int getInstPos( char tokens[10][50],  char *numLinha, int i, int passagem) {
  int instPos = 0, k;
  if (strchr(tokens[0], ':') != NULL) {
    for (k = 1; k < i + 1; k++) {
      if (strchr(tokens[k], ':') != NULL) {
        if (passagem == 1){
          /*erroCompilacao = 1;
          printf("\nERRO >> erro sintático detectado na linha: %s (Dois labels declarados na mesma linha)\n", numLinha);
        */}
        return (k == 1 ? 2 : 1);
      }
    }
    return 1;
  }
  return instPos;
}

int calculaEspaco( char tokens[10][50],  char *numLinha, int instPos, int i) {
  int instrucao = -4, diretiva = -4, espaco = 0, j = 0, errVal = 0, temLinha = 0;
  int expParams;

  if (tokens[i][0] == '(') {
      inMacro = 0;
      temLinha = 1;
    }
    else {
        temLinha = 0;
        if(inMacro == 0) sprintf(numLinha, "%d", atoi(numLinha) + 1);
        inMacro = 1;
    }
    expParams = i - instPos - temLinha;

  if (secText == 1 && strcasecmp(tokens[0], "SECTION") != 0 && i != 0 && tokens[instPos][0] != '(') {
    instrucao = procuraInstrucaoNom(tokens[instPos]); /*verifica qual eh a instrucao*/
    if (instrucao == NAO_ENCONTRADO){
      diretiva = procuraDiretiva(tokens[instPos], expParams);
      if (diretiva == 2 || diretiva == 3 || diretiva == 4){ /*se for space com arg, space sem arg ou const, da erro*/
        /*erroCompilacao = 1;
        printf("\nERRO >> erro semântico detectado na linha: %s (Diretiva '%s' na secao de texto)\n", numLinha, tokens[instPos]);
      */}
      else if (diretiva != NAO_ENCONTRADO){
        return 0; /*se encontrar uma diretiva diferente das acima mencionadas, retorna espaco = 0*/
      }
      else{ /*se nao for nenhuma diretiva dessas acima, entao considera-se uma instrucao desconhecida*/
        /*erroCompilacao = 1;
        printf("\nERRO >> erro semântico detectado na linha: %s (Instrucao desconhecida: '%s')\n", numLinha, tokens[instPos]);
      */}
      return 0;
    } else {
      espaco = instrucoes[instrucao].operandos + 1;
    }
  } else if (secText == 0 && strcasecmp(tokens[0], "SECTION") != 0 && i != 0 && tokens[instPos][0] != '(') {
    diretiva = procuraDiretiva(tokens[instPos], expParams);/*verifica se a diretiva eh valida*/
    if (diretiva  < 0 ) {
      erroCompilacao = 0;
      switch (diretiva) {
        case NAO_ENCONTRADO:
          if (procuraInstrucao(tokens[instPos], expParams) != NAO_ENCONTRADO) printf(" ");
         /* else printf("\nERRO >> erro semântico detectado na linha: %s (Diretiva desconhecida: '%s')\n", numLinha, tokens[instPos]);
         */ break;
        case EXCESSO_OPERANDOS:
          /*printf("\nERRO >> erro sintático detectado na linha: %s (Diretiva com excesso de operandos)\n", numLinha);
          */break;
        case FALTA_OPERANDOS:
          /*printf("\nERRO >> erro sintático detectado na linha: %s (Diretiva com operandos insuficientes)\n", numLinha);
          */break;
        default:
         /*printf("ERRO DE EXECUCAO! Indice da diretiva(%d), linha(%s).\n", diretiva, numLinha);
          */break;
      }
      return 1;
    }
    else {
      espaco = diretivas[diretiva].espaco;
      /*O espaco eh usado diferente para space com parametros, o espaco eh ocupado pelo valor passado como parametro*/
      if (strcasecmp(diretivas[diretiva].nome, "SPACE") == 0 && diretivas[diretiva].operandos == 1) {
        /*verifica se o valor passado como parametro para space eh valido*/
        for (j = 0; j < strlen(tokens[instPos + 1]); j++) {
          if (!isdigit(tokens[instPos + 1][j])) {
            errVal = 1;
            break;
          }
        }
        if (errVal == 0) {
          espaco *= atoi(tokens[instPos + 1]);
        }

      }
    }
  }
  return espaco;
}

void primeiraPassagem(FILE *fp, int NumArgs){
	int i;
	char rotulo[50];
  char tokens[10][50], numLinha[10];
  int simPos = 0, instPos = 0, espaco = 0, valor = 0;
  int simTipo = 0;
  Simbolo simb;

	i = separaTokens(fp, tokens);
  /*remove o '(' do indicador de linha*/
  if(i > -1){
    getNumLinha(numLinha, tokens[i]);

    /*Verifica se a instrucao eh o primeiro ou segundo token*/
    instPos = getInstPos(tokens, numLinha, i, 1);

    /*verifica se os tokens sao validos*/
    validaTokens(i, tokens, numLinha, instPos);

    /*verifica se a secao eh valida*/
    validaSecao(tokens, numLinha);

    espaco = calculaEspaco(tokens, numLinha, instPos, i);


    /*Se o programa receber mais de um ASM, verifica BEGIN e END*/
    if(NumArgs > 1){
      if(strcasecmp(tokens[0], "BEGIN") == 0 || strcasecmp(tokens[1], "BEGIN") == 0) achouBegin = 1;
      else if(strcasecmp(tokens[0], "END") == 0 || strcasecmp(tokens[1], "END") == 0){
        if(achouBegin == 1)
          achouEnd = 1;
        else{
         /* erroCompilacao = 1;
          printf("\nERRO >> erro semântico detectado na linha: %s (END veio antes do BEGIN)\n", numLinha);
        */}
      }
    }

    /*Construcao da tabela de simbolos*/
    if (strchr(tokens[0], ':') != NULL)  {
      strcpy(rotulo, tokens[0]);
      rotulo[strlen(rotulo)-1] = '\0';
      simPos = contPos;

      /*define o tipo de simbolo*/
      if (strcasecmp(tokens[instPos], "CONST") == 0){ /*se for chamado por um const eh constante*/
        simTipo = CONSTANTE;
        valor = atoi(tokens[instPos + 1]);
      }
      if (secText == 1 && strcasecmp(tokens[1], "EXTERN") == 0) simTipo = EXTERN; /*se estiver na secao de texto e for chamado por extern, eh extern kkj*/
      else if (secText == 1) simTipo = LABEL; /*se estiver na secao de texto eh um label*/
      else if (strcasecmp(tokens[1], "BEGIN") == 0 || strcasecmp(tokens[1], "END") == 0) simTipo = LABEL; /*se n estiver na secao de texto, mas se for BEGIN ou END, eh um label*/
      else if (simTipo != CONSTANTE) simTipo = VARIAVEL;/*caso contrario eh variavel*/

      /*verifica se o simbolo ja existe antes de adicionar à tabela*/
      if (procuraSimbolo(rotulo) == NULL) {
        strcpy(simb.nome, rotulo);
        simb.tipo = simTipo;
        if(simTipo == EXTERN) simb.posicao = 0;
        else simb.posicao = simPos;
        simb.valor = valor;
        simb.tam = espaco;
        /*
        printf("nome: %s \ttam: %d\t", simb.nome, simb.tam);
        switch (simb.tipo) {
            case EXTERN:
            printf("EXTERN\n");
            break;
            case LABEL:
            printf("LABEL\n");
            break;
            case VARIAVEL:
            printf("VARIAVEL\n");
            break;
            case CONSTANTE:
            printf("CONSTANTE\n");
            break;
            default:
            printf("ERRO\n");
        }*/
        adicionaSimbolo(simb);
        if (procuraDefinicaoTemp(rotulo) != NULL || strcasecmp(tokens[1], "BEGIN") == 0) adicionaDefinicao(simb); /*se tiver sido declarado como public antes, entao coloca na tabela de definicoes*/
      }
      else {
     /*   erroCompilacao = 1;
        printf("\nERRO >> erro semântico detectado na linha: %s ('%s' declarado pela segunda vez)\n", numLinha, rotulo);
      */}
    }
    else if (strcasecmp(tokens[0], "PUBLIC") == 0){ /*se for declarado como public, entao coloca so o nome na tab de definicoes temporaria*/
      adicionaDefinicaoTemp(tokens[1]);
    }
    else if (strcasecmp(tokens[1], "PUBLIC") == 0){ /*se for declarado como public, entao coloca so o nome na tab de definicoes temporaria*/
      adicionaDefinicaoTemp(tokens[2]);
    }

    /*avanca o contador de posicoes*/
    contPos += espaco;
  }
  else if (secText == 1) {
    /*erroCompilacao = 1;
    printf("\nERRO >> erro semântico detectado na linha: %s (Secao de dados ausente).\n", numLinha);
  */}
}

void verificaBeginEnd(char *arquivoIN){
  if(achouBegin == 0){
   /* erroCompilacao = 1;
    printf("\nERRO >> erro semântico detectado (Nao foi encontrado nenhum BEGIN no arquivo %s)\n", arquivoIN);
  */}

  if(achouEnd == 0){
  /*  erroCompilacao = 1;
    printf("\nERRO >> erro semântico detectado (Nao foi encontrado nenhum END no arquivo %s)\n", arquivoIN);
  */}

  achouBegin = 0;
  achouEnd = 0;
}

/*Recebe um token que tem a possibilidade de ter um '+', e o separa entre o nome do simbolo e o offset */
char* separaTokenDoOffset(char *token, int *offset){
  char *aux, *nome_simbolo;

  nome_simbolo = strtok(token, "+");
  aux = strtok(NULL, "+");
  if(aux != NULL) *offset = atoi(aux);

  return nome_simbolo;
}

int getErroCompilacao(){
  return erroCompilacao;
}

void verificaSecaoAtual( char tokens[10][50]) {
  if (strcasecmp(tokens[0], "section") == 0 && strcasecmp(tokens[1], "text") == 0) secText = 1;
  else if (strcasecmp(tokens[0], "section") == 0 && strcasecmp(tokens[1], "data") == 0) secText = 0;
}

void verificaEspacoAlocado(Simbolo simb, int offset,  char *numLinha){
  if(offset > (simb.tam - 1) && simb.tipo == VARIAVEL){
    erroCompilacao = 0;
    /*printf("\nERRO >> erro semântico detectado na linha: %s (Tentativa de manipulacao de espaco nao alocado.)\n", numLinha);
  */}
}

void verificaStops(){
  if(quantStops == 0){
 /*   erroCompilacao = 1;
    printf("\nERRO >> erro semântico detectado (Nao foi encontrada nenhuma instrucao STOP no programa)\n");
 */ }
}

void segundaPassagem(FILE *fp){
  int i, j, temLinha = 0; /*i eh o maior indice do vetor de tokens da linha dada*/
  char *rotulo;
  char tokens[10][50], numLinha[10], nova_linha[50];
  int instrucao, diretiva, expParams, espaco = 0, instPos = 0, offset = 0, offset2 = 0, operando = 0; /*o operando so eh usado no space e no const*/
  ListSimbolo *Lsimb1 = NULL, *Lsimb2 = NULL;

  i = separaTokens(fp, tokens);
  if(i > -1){ /*Esse if evita bugs no fim da leitura do arquivo*/

    /*remove o '(' do indicador de linha*/
    getNumLinha(numLinha, tokens[i]);

    /*Verifica se a instrucao eh o primeiro ou segundo token (ou seja, o indice 0 ou 1 do vetor de tokens)*/
    instPos = getInstPos(tokens, numLinha, i, 2);

    /*checa se a secao atual eh a de texto ou a de dados, modificando a flag secText*/
    verificaSecaoAtual(tokens);

    espaco = calculaEspaco(tokens, numLinha, instPos, i);

    /*Calcula a quantidade de operandos*/
    if (tokens[i][0] == '(') {
      inMacro = 0;
      temLinha = 1;
    }
    else {
        temLinha = 0;
        if(inMacro == 0) sprintf(numLinha, "%d", atoi(numLinha) + 1);
        inMacro = 1;
    }
    expParams = i - instPos - temLinha;

    if (secText == 1 && strcasecmp(tokens[0], "SECTION") != 0 && i != 0) { /*Se for uma instrucao...*/
      instrucao = procuraInstrucao(tokens[instPos], expParams); /*verifica se a instrucao eh valida*/
      if (instrucao < 0){ /*se o numero de operandos for invalido...*/
       /* switch (instrucao) {
          case EXCESSO_OPERANDOS:
            erroCompilacao = 1;
            printf("\nERRO >> erro sintático detectado na linha: %s (Instrucao '%s' com excesso de operandos)\n", numLinha, tokens[instPos]);
            break;
          case FALTA_OPERANDOS:
            erroCompilacao = 1;
            printf("\nERRO >> erro sintático detectado na linha: %s (Instrucao '%s' com operandos insuficientes)\n", numLinha, tokens[instPos]);
            break;
          case NAO_ENCONTRADO:
            break;
          default:
            erroCompilacao = 1;
            printf("ERRO DE EXECUCAO! Indice da intrucao(%d), linha(%s).\n", instrucao, numLinha);
            break;
        }*/
      }
      else { /*ja que o numero de operandos eh valido, resta agora saber se os tipos deles sao validos, para cada instrucao. Se for, gera codigo objeto*/
        if((instrucao >= 0 && instrucao <= 3) || instrucao == 9 || instrucao == 12){ /*ADD, SUB, MULT, DIV, LOAD, OUTPUT*/
          rotulo = separaTokenDoOffset(tokens[instPos+1], &offset);
          Lsimb1 = procuraSimbolo(rotulo);
          if(Lsimb1 != NULL){
            verificaEspacoAlocado(Lsimb1->simbolo, offset, numLinha);
            if(Lsimb1->simbolo.tipo == VARIAVEL || Lsimb1->simbolo.tipo == EXTERN){
              sprintf(nova_linha, "%s %d ", instrucoes[instrucao].opcode, Lsimb1->simbolo.posicao + offset);
              adicionaLinhaProgFinal(nova_linha);
              adicionaLinhaDeBits("01");
              if (Lsimb1->simbolo.tipo == EXTERN) adicionaUso(Lsimb1->simbolo.nome, contPos + 1);
            }
            else if(Lsimb1->simbolo.tipo == CONSTANTE){
              if(instrucao == 3 && Lsimb1->simbolo.valor == 0){ /*verifica se o operando eh uma constante com valor zero*/
               /* erroCompilacao = 1;
                printf("\nERRO >> erro semântico detectado na linha: %s (Tentativa de divisao por zero!)\n", numLinha);
              */}
              sprintf(nova_linha, "%s %d ", instrucoes[instrucao].opcode, Lsimb1->simbolo.posicao);
              adicionaLinhaProgFinal(nova_linha);
              adicionaLinhaDeBits("01");
            }
            else{
               /*   erroCompilacao = 1;
                  printf("\nERRO >> erro semântico detectado na linha: %s (Instrucao com operandos de tipos invalidos)\n", numLinha);
            */}
          }
          else{
            /*erroCompilacao = 1;
            printf("\nERRO >> erro semântico detectado na linha: %s (Simbolo nao definido!)\n", numLinha);
          */}
        }
        else if(instrucao >= 4 && instrucao <= 7){ /*JMP, JMPN, JMPP, JMPZ*/
          Lsimb1 = procuraSimbolo(tokens[instPos+1]);
          if(Lsimb1 != NULL){
            if(Lsimb1->simbolo.tipo == LABEL || Lsimb1->simbolo.tipo == EXTERN){
              sprintf(nova_linha, "%s %d ", instrucoes[instrucao].opcode, Lsimb1->simbolo.posicao);
              adicionaLinhaProgFinal(nova_linha);
              adicionaLinhaDeBits("01");
              if (Lsimb1->simbolo.tipo == EXTERN) adicionaUso(Lsimb1->simbolo.nome, contPos + 1);
            }
            else{
             /* erroCompilacao = 1;
              printf("\nERRO >> erro semântico detectado na linha: %s (Tentativa de pulo para rotulo com tipo invalido)\n", numLinha);
            */}
          }
          else{
           /* erroCompilacao = 1;
                printf("\nERRO >> erro semântico detectado na linha: %s (Simbolo nao definido!)\n", numLinha);
          */}
        }
        else if(instrucao == 8){ /*COPY*/
          rotulo = separaTokenDoOffset(tokens[instPos+1], &offset);
          Lsimb1 = procuraSimbolo(rotulo);
          rotulo = separaTokenDoOffset(tokens[instPos+2], &offset2);
          Lsimb2 = procuraSimbolo(rotulo);
          if(Lsimb1 != NULL && Lsimb2 != NULL){
            verificaEspacoAlocado(Lsimb1->simbolo, offset, numLinha);
            verificaEspacoAlocado(Lsimb2->simbolo, offset2, numLinha);
            if((Lsimb1->simbolo.tipo == VARIAVEL || Lsimb1->simbolo.tipo == CONSTANTE || Lsimb1->simbolo.tipo == EXTERN) &&
              (Lsimb2->simbolo.tipo == VARIAVEL || Lsimb2->simbolo.tipo == EXTERN)){
              sprintf(nova_linha, "%s %d %d ", instrucoes[instrucao].opcode, Lsimb1->simbolo.posicao + offset, Lsimb2->simbolo.posicao + offset2);
              adicionaLinhaProgFinal(nova_linha);
              adicionaLinhaDeBits("011");
              if (Lsimb1->simbolo.tipo == EXTERN) adicionaUso(Lsimb1->simbolo.nome, contPos + 1);
              if (Lsimb2->simbolo.tipo == EXTERN) adicionaUso(Lsimb1->simbolo.nome, contPos + 2);
            }
            else{
             /* erroCompilacao = 1;
              printf("\nERRO >> erro semântico detectado na linha: %s (Instrucao com operandos de tipos invalidos)\n", numLinha);
            */}
          }
          else{
          /*  erroCompilacao = 1;
                printf("\nERRO >> erro semântico detectado na linha: %s (Simbolo nao definido!)\n", numLinha);
          */}
        }
        else if(instrucao == 10 || instrucao == 11){ /*STORE, INPUT*/
          rotulo = separaTokenDoOffset(tokens[instPos+1], &offset);
          Lsimb1 = procuraSimbolo(rotulo);
          if(Lsimb1 != NULL){
            verificaEspacoAlocado(Lsimb1->simbolo, offset, numLinha);
             if(Lsimb1->simbolo.tipo == VARIAVEL || Lsimb1->simbolo.tipo == EXTERN){
              sprintf(nova_linha, "%s %d ", instrucoes[instrucao].opcode, Lsimb1->simbolo.posicao + offset);
              adicionaLinhaProgFinal(nova_linha);
              adicionaLinhaDeBits("01");
              if (Lsimb1->simbolo.tipo == EXTERN) adicionaUso(Lsimb1->simbolo.nome, contPos + 1);
            }
            else if(Lsimb1->simbolo.tipo == CONSTANTE){
             /* erroCompilacao = 1;
              printf("\nERRO >> erro semântico detectado na linha: %s (Tentativa de mudanca de valor constante)\n", numLinha);
            */}
            else{
            /*  erroCompilacao = 1;
              printf("\nERRO >> erro semântico detectado na linha: %s (Instrucao com operandos de tipos invalidos)\n", numLinha);
            */}
          }
          else{
            /*erroCompilacao = 1;
                printf("\nERRO >> erro semântico detectado na linha: %s (Simbolo nao definido!)\n", numLinha);
          */}
        }
        else if(instrucao == 13){ /*STOP*/
          quantStops++;
          sprintf(nova_linha, "%s ", instrucoes[instrucao].opcode);
          adicionaLinhaProgFinal(nova_linha);
          adicionaLinhaDeBits("0");
        }
      }
    }
    else if (strcasecmp(tokens[0], "SECTION") != 0 && i != 0) { /*Se for uma diretiva...*/
      diretiva = procuraDiretiva(tokens[instPos], expParams);
      switch (diretiva) {
        case 2: /*SPACE sem argumentos*/
          sprintf(nova_linha, "0 ");
          adicionaLinhaProgFinal(nova_linha);
          adicionaLinhaDeBits("0");
          break;
        case 3: /*SPACE com argumentos*/
          if((tokens[instPos+1][0]=='0' && (tokens[instPos+1][1]=='x' || tokens[instPos+1][1]=='X')) ||
            (tokens[instPos+1][1]=='0' && (tokens[instPos+1][2]=='x' || tokens[instPos+1][2]=='X'))) /*Se os primeiros caracteres forem 0x ou 0X ou -0x ou -0X, entao le como hexa*/
            sscanf(tokens[instPos+1], "%x", &operando);
          else sscanf(tokens[instPos+1], "%d", &operando); /*senao, le como decimal mesmo*/
          if(operando > 0){
            for(j=0; j<operando; j++){
              sprintf(nova_linha, "0 ");
              adicionaLinhaProgFinal(nova_linha);
              adicionaLinhaDeBits("0");
            }
          }
          else {
           /* erroCompilacao = 1;
            printf("\nERRO >> erro semântico detectado na linha: %s (Operando invalido para essa diretiva)\n", numLinha);
          */}
          break;
        case 4: /*CONST*/
          if((tokens[instPos+1][0]=='0' && (tokens[instPos+1][1]=='x' || tokens[instPos+1][1]=='X')) ||
            (tokens[instPos+1][1]=='0' && (tokens[instPos+1][2]=='x' || tokens[instPos+1][2]=='X'))) /*Se os primeiros caracteres forem 0x ou 0X ou -0x ou -0X, entao le como hexa*/
            sscanf(tokens[instPos+1], "%x", &operando);
          else sscanf(tokens[instPos+1], "%d", &operando); /*senao, le como decimal mesmo*/
          sprintf(nova_linha, "%d ", operando);
          adicionaLinhaProgFinal(nova_linha);
          adicionaLinhaDeBits("0");
          break;
      }
    }

    /*avanca o contador de posicoes*/
    contPos += espaco;
  }
}

void monta_arquivo_final(char *nomeArquivoIN, FILE *fpOUT){
  char nomePrograma[100];
  ListSimbolo *auxDefinicoes = ldefinicoes;
  ListTabUso *auxUso = luso;
  ListProgramaFinal *auxProgFinal = lprogfinal;
  ListMapaBits *auxMapaBits = lmapabits;

  strcpy(nomePrograma, nomeArquivoIN);
  nomePrograma[strlen(nomePrograma)-4] = '\0'; /*retira o .asm do nome do arquivo*/

  fprintf(fpOUT, "H: %s\n", nomePrograma);
  fprintf(fpOUT, "H: %d\n", tamanhoPrograma);
  fprintf(fpOUT, "H: ");

  /*imprime o mapa de bits*/
  while (auxMapaBits != NULL) {
    fprintf(fpOUT, "%s", auxMapaBits->linha_de_bits);
    auxMapaBits = auxMapaBits->prox;
  }

  fprintf(fpOUT, "\nTD:");

  /*imprime a tabela de definicoes*/
  while (auxDefinicoes != NULL) {
    fprintf(fpOUT, " %s %d", auxDefinicoes->simbolo.nome, auxDefinicoes->simbolo.posicao);
    auxDefinicoes = auxDefinicoes->prox;
  }

  fprintf(fpOUT, "\nTU:");

  /*imprime a tabela de uso*/
  while (auxUso != NULL) {
    fprintf(fpOUT, " %s %d", auxUso->nomeSimboloExtern, auxUso->posicaoDoUso);
    auxUso = auxUso->prox;
  }

  fprintf(fpOUT, "\nT: ");

  /*imprime o programa final*/
  while (auxProgFinal != NULL) {
    fprintf(fpOUT, "%s", auxProgFinal->linha);
    auxProgFinal = auxProgFinal->prox;
  }
}

char* convert_asm_to_o(char nomeArquivo[]){
  char parte_asm[] = ".asm";
  char* ponto_de_mudanca;

  ponto_de_mudanca = strstr(nomeArquivo, parte_asm);

  if(ponto_de_mudanca == NULL){
    printf("ERRO! Arquivo dado em formato invalido!\n");
    exit(1);
  }
  else {
    strcpy(ponto_de_mudanca, ".o\0");
  }
  return nomeArquivo;
}


void duasPassagens(char *nomeArquivoIN, int NumArgs){
  FILE *fpIN, *fpOUT;
  char *nomeArquivoOUT, aux[100];

  strcpy(aux, nomeArquivoIN);

  nomeArquivoOUT = convert_asm_to_o(aux);

  fpIN = fopen(nomeArquivoIN, "r");
  if(fpIN == NULL){
    printf("Erro ao abrir o arquivo!\n");
    return;
  }
  fpOUT = fopen(nomeArquivoOUT, "w");
  if(!fpOUT){
    printf("Erro ao abrir o arquivo!\n");
    return;
  }

  while (!feof(fpIN)) primeiraPassagem(fpIN, NumArgs);
  if(NumArgs > 1) verificaBeginEnd(nomeArquivoIN);
  /*imprimeSimbolos();*/
  rewind(fpIN);
  resetInMacro();
  tamanhoPrograma = contPos;
  contPos = 0;
  secText = 0;

  while (!feof(fpIN)) segundaPassagem(fpIN);
  verificaStops();

  monta_arquivo_final(nomeArquivoIN, fpOUT);

  esvaziaTabelas();
  fclose(fpIN);
  fclose(fpOUT);

  /*if(getErroCompilacao() == 1){
    remove(nomeArquivoOUT);
    printf("Devido aos erros detectados, o objeto do arquivo %s nao foi gerado.\n", nomeArquivoIN);
  }*/

  /*zerando as variaveis globais para serem usadas novamente*/
  contPos = 0;
  secText = 0;
  erroCompilacao = 0;
  quantStops = 0;
  achouBegin = 0;
  achouEnd = 0;

  return;
}

void resetInMacro() {
  inMacro = 0;
}
