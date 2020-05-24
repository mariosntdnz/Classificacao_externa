#include <stdio.h>
#include <stdlib.h>
typedef struct registros
{
	int chave;
}Registro;

typedef struct pega_posi_register
{
	Registro r;
	int      i;

}Indice_register;

int selec_natural(int max,FILE *file);
void set_registro(Registro *r,int max);
Indice_register menor_part(Registro *r,int max);
int compare_menor(int a,int b);