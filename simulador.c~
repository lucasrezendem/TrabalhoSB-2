#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "simulador.h"

void simula(int16_t *mem, int tamanho){
	int16_t pc=0, acc=0;
	char texto_input[7]; /*tamanho 7 pois o numero pode ter no maximo 5 digitos decimais + o sinal, e no maximo 4 digitos hexadecimais + sinal + 0x = 7*/
	int aux;
	/*aux eh usado somente para o input*/

	while (pc<tamanho){
		switch(mem[pc]){
			case 1: /*ADD*/
				acc += mem[mem[pc+1]];
				printf("(%d)\tACC <- ACC + (mem[%d] = %d)\n", pc, mem[pc+1], mem[mem[pc+1]]);
				pc += 2;
				break;

			case 2: /*SUB*/
				acc -= mem[mem[pc+1]];
				printf("(%d)\tACC <- ACC - (mem[%d] = %d)\n", pc, mem[pc+1], mem[mem[pc+1]]);
				pc += 2;
				break;

			case 3: /*MULT*/
				acc *= mem[mem[pc+1]];
				printf("(%d)\tACC <- ACC * (mem[%d] = %d)\n", pc, mem[pc+1], mem[mem[pc+1]]);
				pc += 2;
				break;

			case 4: /*DIV*/
				if(mem[mem[pc+1]] == 0){
					printf("(%d)\tERRO: TENTATIVA DE DIVISAO POR ZERO! FIM DA EXECUCAO.\n", pc);
					return;
				}
				acc /= mem[mem[pc+1]];
				printf("(%d)\tACC <- ACC / (mem[%d] = %d)\n", pc, mem[pc+1], mem[mem[pc+1]]);
				pc += 2;
				break;

			case 5: /*JMP*/
				printf("(%d)\tPC <- %d\n", pc, mem[pc+1]);
				pc = mem[pc+1];
				break;

			case 6: /*JMPN*/
				printf("(%d)\tSE ACC < 0, PC <- %d\n", pc, mem[pc+1]);
				if(acc < 0) pc = mem[pc+1];
				else pc += 2;
				break;

			case 7: /*JMPP*/
				printf("(%d)\tSE ACC > 0, PC <- %d\n", pc, mem[pc+1]);
				if(acc > 0) pc = mem[pc+1];
				else pc += 2;
				break;

			case 8: /*JMPZ*/
				printf("(%d)\tSE ACC = 0, PC <- %d\n", pc, mem[pc+1]);
				if(acc == 0) pc = mem[pc+1];
				else pc += 2;
				break;

			case 9: /*COPY*/
				mem[mem[pc+2]] = mem[mem[pc+1]];
				printf("(%d)\tmem[%d] <- (mem[%d] = %d)\n", pc, mem[pc+2], mem[pc+1], mem[mem[pc+1]]);
				pc += 3;
				break;

			case 10: /*LOAD*/
				acc = mem[mem[pc+1]];
				printf("(%d)\tACC <- (mem[%d] = %d)\n", pc, mem[pc+1], mem[mem[pc+1]]);
				pc += 2;
				break;

			case 11: /*STORE*/
				mem[mem[pc+1]] = acc;
				printf("(%d)\t(ACC = %d) -> mem[%d]\n", pc, acc, mem[pc+1]);
				pc += 2;
				break;

			case 12: /*INPUT*/
				printf("(%d)\tINPUT: mem[%d] <- ", pc, mem[pc+1]);
				scanf("%s", texto_input);
				getchar();

				if((texto_input[0]=='0' && (texto_input[1]=='x' || texto_input[1]=='X')) ||
					(texto_input[1]=='0' && (texto_input[2]=='x' || texto_input[2]=='X'))) /*Se os primeiros caracteres forem 0x ou 0X ou -0x ou -0X, entao le como hexa*/
					sscanf(texto_input, "%x", &aux);
				else sscanf(texto_input, "%d", &aux); /*senao, le como decimal mesmo*/

				printf("INPUT = %d\n", aux);

				while(aux > 32767 || aux < -32768){
					printf("ERRO: O VALOR PRECISA TER NO MAXIMO 16 BITS!\n");
					printf("(%d)\tINPUT: mem[%d] <- ", pc, mem[pc+1]);
					scanf("%d", &aux);
					getchar();
				}
				mem[mem[pc+1]] = aux;
				pc += 2;
				break;

			case 13: /*OUTPUT*/
				printf("(%d)\tOUTPUT: mem[%d] = %d\n", pc, mem[pc+1], mem[mem[pc+1]]);
				pc += 2;
				break;

			case 14: /*STOP*/
				printf("(%d)\tSTOP\n", pc);
				return;

			default:
				printf("(%d)\tERRO: INSTRUCAO NAO ENCONTRADA!\n", pc);
				pc += 1;
				return;
		}
	}

	printf("(%d)\tERRO: PROGRAMA TERMINOU SEM A INSTRUCAO STOP!\n", pc);
}
