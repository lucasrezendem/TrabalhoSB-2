#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "realocador.h"
#include "simulador.h"

int main(int argc, char *argv[]) {
    /*Verifica a quantidade de argumentos*/
    if (argc < 3) {
        printf("Erro: argumentos insuficientes\n");
        printf("Uso correto: ./carregador [programa] [numChunks] [chunks] [tamanhos]\n");
        exit(1);
    }

    /*Verifica se o numero de chunks é de fato um numero*/
    int i;
    for (i = 0; i < strlen(argv[2]); i++) {
        if(!isdigit(argv[2][i])) {
            printf("Erro: numero de chunks invalido - o valor do numero de chunks deve ser um numero\n");
            exit(1);
        }
    }

    int chunks = atoi(argv[2]);

    if (argc < 3 + 2*chunks) {
        printf("Erro: argumentos insuficientes\n");
        printf("Uso correto: ./carregador [programa] [numChunks] [chunks] [tamanhos]\n");
        exit(1);
    }
    if (argc > 3 + 2*chunks) {
        printf("Erro: argumentos em excesso\n");
        printf("Uso correto: ./carregador [programa] [numChunks] [chunks] [tamanhos]\n");
        exit(1);
    }

    /*Validar arquivo*/
    FILE *fp = fopen(argv[1], "r");
    if(!fp) {
        printf("Erro: arquivo não encontrado\n");
        exit(1);
    }
    fclose(fp);

    /*Validar tamanhos e locais*/
    int j;
    for (j = 3; j < argc; j++) {
        for (i = 0; i < strlen(argv[j]); i++) {
            if(!isdigit(argv[j][i])) {
                printf("Erro: valor invalido - os valores dos tamnhos dos chunks e dos ederecos devem ser numeros\n");
                exit(1);
            }
        }
    }
    ProgInfo info;
    MemTipo mem;
    int16_t *codigo;
    int espaco;

    int *tamanhos = (int *) malloc(chunks*sizeof(int));
    int *locais = (int *) malloc(chunks*sizeof(int));

    for(j = 0; j < chunks; j++) {
        tamanhos[j] = atoi(argv[j+3]);
        locais[j] = atoi(argv[j+chunks+3]);
    }

    novaInfo(&info);
    recuperaInformacoes(argv[1], &info);

    initMem(&mem, chunks, tamanhos, locais);

    codigo = extraiCodigo(&info);

    printf("tam ex:%s\n", info.tamanho);

    printf("Codigo:\n");
    for(i = 0; i < atoi(info.tamanho); i++) {
        printf("%3hd ", codigo[i]);
    }
    printf("\n");

    simula(codigo, atoi(info.tamanho));

    espaco = verificaEspaco(&mem, atoi(info.tamanho));

    if (!espaco) {
        printf("OUT OF MEMORY - YOUR PROGRAM WILL NOT BE LOADED\n");
    }
    else {
        realoca(&mem, &info, codigo, espaco);
        escreveSaida(codigo, &info);
    }

    free(tamanhos);
    free(locais);
    free(codigo);
    liberaInfo(&info);
    return 0;
}
