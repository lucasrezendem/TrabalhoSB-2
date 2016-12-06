#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"

void recuperaInformacoes(char *arquivo, ProgInfo *info) {
    int tam;

    FILE *fp = fopen(arquivo, "r");
    if (fp == NULL) {
        printf("ERRO AO ABRIR O ARQUIVO!\n");
        exit(1);
    }

    fgets(info->nome, sizeof(info->nome), fp);
    fgets(info->tamanho, sizeof(info->tamanho), fp);

    /* memmove basicamente pra retirar os primeiros 3 caracteres ("H: ") da
       string e deixar apenas a informacao pertinente, isso tambem explica o
       porque de eu definir o limite como 5 caracteres a menos, os 3 chars +
       \n e \0*/
    memmove(info->nome, info->nome + 3*sizeof(char), strlen(info->nome));
    memmove(info->tamanho, info->tamanho + 3*sizeof(char), strlen(info->tamanho));

    tam = atoi(info->tamanho) + 5; /* de novo os 5 chars*/

    info->realocacao = (char *) malloc(tam*sizeof(char));
    if (info->realocacao == NULL) {
        printf("Erro ao alocar memoria\n");
        exit(1);
    }

    fgets(info->realocacao, tam, fp);
    memmove(info->realocacao, info->realocacao + 3, strlen(info->realocacao));

    /* para cada instrucao podem haver no maximo 2 digitos e um espaco
       logo multiplico o info->tamanho por 3*/
    info->codigo = (char *) malloc(3*tam*sizeof(char));
    if (info->codigo == NULL) {
        printf("Erro ao alocar memoria\n");
        exit(1);
    }

    fgets(info->codigo, 3*tam, fp);
    memmove(info->codigo, info->codigo + 3, strlen(info->codigo));

    /*retirar "\n"*/
    info->nome[strlen(info->nome) - 1] = '\0';
    info->tamanho[strlen(info->tamanho) - 1] = '\0';
    info->realocacao[strlen(info->realocacao) - 1] = '\0';
    info->codigo[strlen(info->codigo) - 1] = '\0';

    fclose(fp);
}

void liberaInfo(ProgInfo *info) {
    if (info->realocacao != NULL) free(info->realocacao);
    info->realocacao = NULL;
    if (info->codigo != NULL) free(info->codigo);
    info->codigo = NULL;

}

void novaInfo(ProgInfo *info) {
    info->realocacao = NULL;
    info->codigo = NULL;
}

int16_t *extraiCodigo(ProgInfo *info) {
    int16_t *cod;
    int tam, int_aux;
    char aux[5];

    tam = atoi(info->tamanho);

    cod = (int16_t *) malloc(tam*sizeof(int16_t));
    if(!cod) {
        printf("Erro ao alocar memoria\n");
        exit(1);
    }

    /*Como o sscanf nao anda na string j faz a movimentacao ao longo da string*/
    int i, j;
    for(i = 0, j = 0 ; i < tam; i++, j += strlen(aux) + 1) {
        sscanf(info->codigo + j, "%s", aux);
        int_aux = atoi(aux);
        if(int_aux > 32767 || int_aux < -32768){
            printf("ERRO: VALOR COM MAIS DE 16 BITS ENCONTRADO!\n");
            free(cod);
            exit(1);
        }
        cod[i] = int_aux;
    }
    return cod;
}
