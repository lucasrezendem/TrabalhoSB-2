#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "parser.h"
#include "realocador.h"

/*lembrar de desalocar no programa principal*/

int main(int argc, char *argv[]) {
    ProgInfo info;
    MemTipo mem;
    int16_t *codigo;
    int i, espaco;
    int chunks = 3;
    int tamanhos[3] = {12, 3, 7};
    int locais[3] = {100, 150, 280};

    novaInfo(&info);
    recuperaInformacoes("PROGA", &info);

    initMem(&mem, chunks, tamanhos, locais);

    codigo = extraiCodigo(&info);

    for(i = 0; i < atoi(info.tamanho); i++) {
        printf("%3d ", codigo[i]);
    }
    printf("\n");

    espaco = verificaEspaco(&mem, atoi(info.tamanho));

    realoca(&mem, &info, codigo, espaco);

    for(i = 0; i < atoi(info.tamanho); i++) {
        printf("%3d ", codigo[i]);
    }
    printf("\n");

    free(codigo);
    liberaInfo(&info);
    return 0;
}
