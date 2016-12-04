#ifndef REALOCADOR_H
#define REALOCADOR_H

#define NO_SPACE 0
#define MULTIPLE_CHUNKS -1
/*Funcoes responsaveis pela alocação do programa principal*/

/*Registros das informacoes de memoria*/
typedef struct memTipo {
    int chunks;
    int *tamanhos;
    int *locais;
} MemTipo;

/*inicializa o registro das memorias com os valores dados*/
void initMem(MemTipo *mem, int chunks, int *tamanhos, int *locais);

/*Recebe o tamanho do programa e verifica se é possivel alocá-lo
  Se não for possivel, retorna 0
  Se for possivel alocar tudo para um unico chunk retorna qual seu indice + 1
  Se for preciso mais de um chunk retorna -1*/
int verificaEspaco(MemTipo *mem, int tamProg);

/*Realoca de acordo com o resultado da verificacao*/
void realoca(MemTipo *mem, int16_t *codigo, int modo);

#endif
