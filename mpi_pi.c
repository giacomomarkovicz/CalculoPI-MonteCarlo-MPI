#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double gerar_pontos (int pontos);
#define PONTOS 10000000   
#define RODADAS 50  
#define MASTER 0        /* ID de processo master */

int main (int argc, char *argv[]) {
double	pi_atual,         			// valor do PI no processo atual
		soma_pi_processos, pi,	        // soma de PI de todos os processos			        				
		media_pi;	        		// media de pi das iterações
int	id_processo,	        // id de processo
	n_processos,       // numero de processos
	i;

	clock_t begin = clock();

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&n_processos); //numero de processos
	MPI_Comm_rank(MPI_COMM_WORLD,&id_processo); //id de processo
	printf ("Processo %d iniciado...\n", id_processo);

	media_pi = 0;
	for (i = 0; i < RODADAS; i++) {

		/* Cada processo executa iterações da função gerar_pontos e calcula a aproximação de pi  */
		pi_atual = gerar_pontos(PONTOS);

		/* MPI_REDUCE irá somar todos os valores de pi_atual retornados de todos os processos    
		* Processo MASTER guarda esse valor em soma_pi_processos 
		* - pi_atual é o valor de pi que cada processo obteve
		* - soma_pi_processos recebe o resutado dessa redução (soma)
		* - tamanho de soma_pi_processos
		* - MPI_DOUBLE é o tipo de dado de som_pi_processos
		* - MPI_SUM função pré definida do MPI (soma os valores dos processos e salva em soma_pi_processos)
		* - MASTER processo que recebe o valor de pi das aproximações das execuções dos outros processos
		* - MPI_COMM_WORLD grupo dos processos participantes.
		*/

		MPI_Reduce(&pi_atual, &soma_pi_processos, 1, MPI_DOUBLE, MPI_SUM,
		MASTER, MPI_COMM_WORLD);

			/* Processo master calcula a media de pi de todos os processos e suas iterações */
			if (id_processo == MASTER) {
				pi = soma_pi_processos/n_processos;
				media_pi = ((media_pi * i) + pi)/(i + 1); 
				printf("Apos %8d pontos gerados, a média do PI está em = %10.8f\n",
				(PONTOS * (i + 1)),media_pi);
			}    
	} 
	if (id_processo == MASTER) {
		printf ("\n	   Valor de PI real PI: 3.1415926535897 \n");

		clock_t end = clock();
		double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		printf("Tempo de execução: %f segundos\n", time_spent);
	}

	MPI_Finalize();

	return 0;
}

double gerar_pontos(int pontos) {
#define sqr(x)	((x)*(x))
long random(void);
double x_coord, y_coord, pi, r; 
int acertos, n;

	acertos = 0;

	/* gerando todos os pontos e distribuindo aleatoriamente */
	for (n = 1; n <= pontos; n++)  {

		/* gera pontos aleatorios para as coordenadas x e y */
		r = (double)random()/RAND_MAX;
		x_coord = (2.0 * r) - 1.0;
		r = (double)random()/RAND_MAX;
		y_coord = (2.0 * r) - 1.0;
		/* adiciona a variavel acertos se...*/
		if ((sqr(x_coord) + sqr(y_coord)) <= 1.0) {
			acertos++;
		}
	}

	pi = 4.0 * (double)acertos/(double)pontos;
	return(pi);
} 
