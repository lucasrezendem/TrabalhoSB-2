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

void realoca(MemTipo *mem, int16_t *codigo, int modo) {
    /*TODO*/
}
