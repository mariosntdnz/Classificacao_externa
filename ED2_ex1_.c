#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ED2_ex1_.h"

#define  SENTINELA 2147483647

/*

	Compilação - gcc ED2_ex1_.c ED2_ex1_.h -o <nome_executavel>

*/

void escreve_na_tela(FILE* arq)
{
	rewind(arq);
	int num;

	//percorre o arquivo ate o final pegando o valor em binario e exibe na tela
	while(1)
	{

		fread(&num, sizeof(int), 1, arq);
		if(feof(arq))
			break;
		printf("%d ", num);


	}
	printf("\n");
}

void escreve_memoria_na_part(Registro *memoria,FILE *part,int max){
/*função responsável por quando o reservatório está cheio ou
  o arquivo chegar ao fim escrever o conteúdo "todo de uma vez" na partição*/

Indice_register index;
	int i = 0;
	for (; i < max - 1; i++)
	{
		index 			       = menor_part(memoria,max);
		//a cada iteração pega o menor e escreve na partição atual
		memoria[index.i].chave = SENTINELA; //-> Para não pegar o menor em toda iteração
		//fprintf(part, "%d\n",index.r.chave );
		fwrite(&index.r.chave, sizeof(int), 1, part);
	}

}

int num_de_part(FILE *f,int max,int decisao){
//se recebe o último parâmetro 1 retorna o nº maximo de partições que terão
//se recebe o último parâmetro 0 retorna a quantidade de registros no arquivo
int a,cont = 0;

	//while(fscanf(f,"%d",&a) != EOF) cont++;
	while(feof(f) == 0){

		fread(&a, sizeof(int), 1, f);
		cont++;
	}
	if(decisao)
		return (cont/(max*2,72)) + 1;
	return cont-1;
}

void set_registro(Registro *r,int max){
//seta o reservatorio com -1 para obter-se controle sobre o reservatorio
	int i = 0;
	for (; i < max; ++i)
		r[i].chave = -1;

}

Registro* alloca_registro(){
//aloca um registro
	Registro *aux = (Registro*) malloc(sizeof(Registro));
	return aux;
}

Indice_register menor_part(Registro *r,int max)
{
//retorna a menor chave da memoria e o indice correspondente a ela
Registro        aux;
Indice_register index;
int             menor;

	menor   = r[0].chave;
	index.i = 0;
	int i = 1;
	for(; i < max ; i++ )
	if(!compare_menor(menor,r[i].chave)) {
		menor = r[i].chave;index.i = i;
	}
	index.r.chave = menor;
	return index;
}

int compare_menor(int a,int b)
{
//retorna 1 se a < b e 0 caso contrário

	if(a < b) return 1;
	return 0;
}

int selec_natural(int max,FILE *file)
{

int 	 		carrega1,controle_arquivo,len_num_arquivo,cont_reservatorio,controle_registros_lidos,max_registros;
char	 		nome_arquivo[num_de_part(file,max,1) + 5],*num;
FILE     		*part;
Indice_register ind_r;
Registro 		*memoria,*reservatorio,*aux;

				rewind(file);
				carrega1 		  		 = 1; //1° vez que entra no arquivo e são lidos n registros de uma vez
				controle_arquivo  		 = 1; //saber numero da partição atual
				len_num_arquivo   		 = 0; //quantos caracteres tem o numero da versao atual
				cont_reservatorio 		 = 0; //o indice atual do reservatório
				controle_registros_lidos = max-1; //quantos registros foram lidos
				max_registros			 = num_de_part(file,max,0); //numero total de registros

				rewind(file); //setando o ponteiro para o como do arquivo, pois a função "num_de_part" já leu o arquivo

				memoria			 = (Registro*) malloc(sizeof(Registro)*max);
				reservatorio	 = (Registro*) malloc(sizeof(Registro)*max);

				aux   = alloca_registro();

				sprintf(nome_arquivo,"part_%d",controle_arquivo); //colando o numero da partição no nome do arquivo

				set_registro(memoria,max); //setando a memória
				set_registro(reservatorio,max); //setando o reservatorio

				while(1){

					controle_registros_lidos++;

					if(carrega1){
					    //Só entra nesse if na primeira vez que é executado
					    int i = 0;
						for( ; i < max; i++ ){

							//fscanf(file,"%d",&memoria[i].chave);
							fread(&memoria[i].chave, sizeof(int), 1, file);
							/*Joga os N registros que cabem na memória para a memória*/

						}

						ind_r = menor_part(memoria,max);
						part  = fopen(nome_arquivo,"wb");
						//fprintf(part,"%d\n",ind_r.r.chave);
						fwrite(&ind_r.r.chave, sizeof(int), 1, part);

						/*As 3 linhas a cima respectivamente, acham o menor, abre a primeira partição e escreve o menor na partição*/

						carrega1 = 0; // para não entrar mais nesse bloco condicional
						continue; //pula p próxima iteração
					}


					//fscanf(file,"%d",&aux->chave);
					fread(&aux->chave, sizeof(int), 1, file); //lê o próximo elemento do arquivo

					if(ind_r.r.chave > aux->chave){

						reservatorio[cont_reservatorio].chave = aux->chave;
						cont_reservatorio++;
						/*Se o elemento lido for menor que o último elemento gravado na partição
						  vai para o reservatório e o indice do reservatório é incrementado*/

					}
					else{

						memoria[ind_r.i].chave  = aux->chave; //->ind_r.i éó indice do ultimo menor elemento
						ind_r = menor_part(memoria,max);
						fwrite(&ind_r.r.chave, sizeof(int), 1, part);
						/*Caso o último elemento lido seja maior que o último elemento gravado na partição
						  coloca ele na memória no lugar do último elemento(no seu respectivo indice)*/

					}

					if(cont_reservatorio == max || controle_registros_lidos == max_registros){
						//Se o reservatório estiver cheio ou li o último registro entra nesse if

						memoria[ind_r.i].chave = SENTINELA; //o ultimo menor vira sentinela para não ser escolhido de novo

						escreve_memoria_na_part(memoria,part,max); //escreve o que sobrou na memoria na partição atual

						fclose(part); //fecha o arquivo...pois será criada uma nova partição

						controle_arquivo++; //número da nova partição

						if(cont_reservatorio == 0) break; /*só entra nesse if caso o reservatório esteja vazio e tiver lido todos registros
															se isso aconteceu, posso encerrar o programa*/

						sprintf(nome_arquivo,"part_%d",controle_arquivo); //nova partição
						part = fopen(nome_arquivo,"wb");
						int i = 0;
						for (; i < cont_reservatorio; i++) {

							//if(reservatorio[i].chave == -1) continue;
							memoria[i].chave = reservatorio[i].chave;
							//coloca na memoria os dados do reservatorio
						}

						if(controle_registros_lidos == max_registros && cont_reservatorio != 0){

							sprintf(nome_arquivo,"part_%d",controle_arquivo);
							part = fopen(nome_arquivo,"wb");
							escreve_memoria_na_part(memoria,part,cont_reservatorio + 1);
							fclose(part);
							controle_arquivo++;
							break;

							/*Nesse bloco caso tiver lido todos os registros e o reservatótio não estiver vazio*
							  cria-se uma nova partição e é colocado os dados presentes no reservatório nela*/

						}



						set_registro(reservatorio,max); //seta o reservatório quando ele está cheio e não chegou ao fim do arquivo

						ind_r = menor_part(memoria,max);

						//fprintf(part, "%d\n",ind_r.r.chave);
						fwrite(&ind_r.r.chave, sizeof(int), 1, part);
						cont_reservatorio = 0; //seta o indice do reservatorio

					}

					if(controle_registros_lidos > max_registros) break;


				}
				int i = 1;
				for (; i < controle_arquivo; i++)
				{
					sprintf(nome_arquivo,"part_%d",i);
					printf("%s : \n",nome_arquivo );
					part = fopen(nome_arquivo,"rb");
					escreve_na_tela(part);

					/*Bloco para imprimir no terminal as partições obtidas*/

				}

				return 1;


}

int main()
{

int  max;
char arq_aux[20];
FILE *arquivo;

	fprintf(stderr,"\nDigite o número máximo de registros a serem carregados na memória por vez : \n");
	scanf("%d",&max);

	fprintf(stderr,"\nDigite o nome do arquivo [digite o path caso esteja em uma pasta diferente] :\n");
	scanf("%s",arq_aux);

	arquivo = fopen(arq_aux,"rb");

	if(selec_natural(max,arquivo)) fprintf(stderr, "\nClassificação feita com sucesso\n");
	else						   fprintf(stderr, "\nErro inesperado\n");
}
