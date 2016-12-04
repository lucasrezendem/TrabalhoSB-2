/*Funcoes responasveis por extrair as informações do programa ligado*/

typedef struct progInfo {
    char nome[260]; /*o nome do programa só pode ter no maximo 255 caracters*/
    char tamanho[17]; /*o tamanho do programa só pode ter no maximo 12 digitos*/
    char *realocacao; /*a informacao de realocacao depende do tamanho*/
    char *codigo; /*o codigo tambem*/
} ProgInfo;

/*Recupera as informacoes do programa em forma de string*/
void recuperaInformacoes(char *arquivo, ProgInfo *info);

/*Libera as alocacoes dinamicas de um ProgInfo*/
void liberaInfo(ProgInfo *info);

/*Inicializa um novo ProgInfo*/
void novaInfo(ProgInfo *info);
