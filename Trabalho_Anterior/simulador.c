#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int conta_tamanho(FILE *fp){
	char aux[5];
	int tam=0;

	while(!feof(fp)){
		fscanf(fp, "%s", aux);
		tam++;
	}

	return tam-1; /*Assim como no exemplo dado pelo professor, o ultimo caracter do programa precisa ser um espaco. */
	/*O -1 serve pra compensar o EOF que eh pego sozinho depois da ultima barra de espaco*/
	/*caso nao tenha a ultima barra de espaco, o fscanf vai puxar o EOF junto com o ultimo numero, ai o -1 nao seria necessario*/
}

/*preenche o vetor mem com cada um dos valores contidos no ".o", cada numero mapeado no indice do vetor correspondente ao endereco do mesmo*/
int preenche_vetor_mem(FILE *fp, int16_t *mem, int tamanho){
	int i, int_aux;

	for(i=0; i<tamanho; i++){
		fscanf(fp, "%d", &int_aux);
		mem[i] = int_aux;
		/*printf("[%d]: %d  %d \n", i, mem[i], int_aux);*/
		if(int_aux > 32767 || int_aux < -32768){
			printf("ERRO: VALOR COM MAIS DE 16 BITS ENCONTRADO!\n");
			return -1;
		}
	}
	return 0;
}

void run(int16_t *mem, int tamanho){
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

int main(int argc, char *argv[]){
	if(argc > 2){
		printf("ERRO: O SIMULADOR ESPERA RECEBER SOMENTE O NOME DO ARQUIVO A SER SIMULADO COMO ARGUMENTO!\n");
		return -1;
	} else if(argc == 1){
		printf("ERRO: O SIMULADOR ESPERA RECEBER O NOME DO ARQUIVO DO PROGRAMA A SER SIMULADO COMO ARGUMENTO!\n");
		return -1;
	}

	FILE *fp;
	int tamanho;
	fp = fopen(argv[1], "r");	
	if(!fp){
		printf("ERRO AO ABRIR O ARQUIVO!\n");
		return -1;
	}
	tamanho = conta_tamanho(fp);
	int16_t mem[tamanho]; /*Professor Guilherme provavelmente nao curtiria isso, mas eu acredito que o ganho em fazer isso supera os riscos rsrs*/ 

	rewind(fp);

	if(preenche_vetor_mem(fp, mem, tamanho))
		return -1;
	/*Caso ache algum valor acima de 16 bits, para a execucao e indica o problema*/

	run(mem, tamanho);

	fclose(fp);

	return 0;
}