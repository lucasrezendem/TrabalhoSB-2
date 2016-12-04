#include <stdlib.h>
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
            codigo[i] += mem->locais[currChunk] + j;
        }
    }
}
