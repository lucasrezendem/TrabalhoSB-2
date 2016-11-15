#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "passagens.h"

int main(int argc, char const *argv[]) {
  FILE *fp, *fpfinal;
  fp = fopen("triangulo.mcr", "r");
  if(!fp){
    printf("ERRO AO ABRIR O ARQUIVO!\n");
    return -1;
  }
  fpfinal = fopen("triangulo.o", "w");
  if(!fpfinal){
    printf("ERRO AO ABRIR O ARQUIVO!\n");
    return -1;
  }

  while (!feof(fp)) primeiraPassagem(fp);
  imprimeSimbolos();
  rewind(fp);
  while (!feof(fp)) segundaPassagem(fp, fpfinal);
  verificaStops();

  esvaziaTabela();
  fclose(fp);
  fclose(fpfinal);

  if(getErroCompilacao() == 1){
    remove("triangulo.o");
    printf("Devido aos erros detectados, o arquivo objeto nao foi gerado.\n");
  }

  return 0;
}
