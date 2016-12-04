#include <stdlib.h>
#include <string.h>
#include "realocador.h"

void initMem(MemTipo *mem, int chunks, int *tamanhos, int *locais) {
    mem->chunks = chunks;
    mem->tamanhos = tamanhos;
    mem->locais = locais;
}

int verificaEspaco(MemTipo *mem, int tamProg) {
    int tamDisp = 0;
    int i;

    /*Verifica se o programa cabe em algum chunk e calcula espaco total*/
    for(i = 0; i < mem->chunks; i++) {
        if(mem->tamanhos[i] >= tamProg) return i + 1;
        tamDisp += mem->tamanhos[i];
    }

    /*Verifica se o programa cabe no espaco total*/
    if(tamDisp < tamProg) return NO_SPACE;

    return MULTIPLE_CHUNKS;
}

void realoca(MemTipo *mem, ProgInfo *info, int16_t *codigo, int modo) {
    int currChunk = 0;
    int i, j;
    int tam = atoi(info->tamanho);

    if (modo > 0) currChunk = modo - 1;

    for(i = 0, j = 0; i < tam; i++, j++) {
        if(j == mem->tamanhos[currChunk]) {
            j = 0;
            currChunk++;
        }
        if(info->realocacao[i] == '1') {
            codigo[i] += mem->locais[currChunk];
            if (codigo[i] > 32767 || codigo[i] < -32768) {
                printf("Erro: overflow no valor do código\n");
                exit(1);
            }
        }
    }
}

void escreveSaida(int16_t *codigo, ProgInfo *info) {
    char *arqNome;

    arqNome = malloc((strlen(info->nome) + 4) * sizeof(char));

    strcpy(arqNome, info->nome);
    strcat(arqNome, ".im");

    FILE *fp = fopen(arqNome, "w+");
    if (!fp) {
        printf("Erro ao ecrever arquivo de saída\n");
        free(arqNome);
        exit(1);
    }

    int i;
    for (i = 0; i < atoi(info->tamanho); i++) {
        fprintf(fp, "%d ", codigo[i]);
    }
    fprintf(fp, "\n");

    free(arqNome);
}
