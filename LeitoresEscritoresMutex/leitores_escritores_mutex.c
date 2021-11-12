/* Bruno Esteves 17/0100863 */

#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

#define TRUE 1

#define NE 3 //numero de escritores
#define NL 10 //numero de leitores

pthread_mutex_t lock_bd = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_nl = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t lock_solucao = PTHREAD_MUTEX_INITIALIZER;

int num_leitores = 0;

void * reader(void *arg);
void * writer(void *arg);
void read_data_base();
void use_data_read();
void think_up_data();
void write_data_base();

int main() {

	pthread_t r[NL], w[NE];
	int i;
    int *id;
    /* criando leitores */
    for (i = 0; i < NL ; i++) {
	    id = (int *) malloc(sizeof(int));
        *id = i;
		pthread_create(&r[i], NULL, reader, (void *) (id));
	}

	 /* criando escritores */
	for (i = 0; i< NE; i++) {
	    id = (int *) malloc(sizeof(int));
        *id = i;
	    pthread_create(&w[i], NULL, writer, (void *) (id));
	}
	pthread_join(r[0],NULL);
	return 0;
}

void * reader(void *arg) {
	int i = *((int *) arg);
	while(TRUE) {               /* repete para sempre */
		pthread_mutex_lock(&lock_solucao); /* lock usado para impedir a "criacao" de mais um leitor enquanto estiverem escrevendo no bd */ 
									       /* com isso criamos uma exclusão mútua para travar o lock_bd entre os escritores e leitores */ 
			pthread_mutex_lock(&lock_nl); /* tenho que lockar por que num_leitores eh variavel compartilhada */
				num_leitores++; 
				if(num_leitores == 1){
					pthread_mutex_lock(&lock_bd);
				}
			pthread_mutex_unlock(&lock_nl);
		pthread_mutex_unlock(&lock_solucao);

        read_data_base(i);       /* acesso aos dados */
		 
        pthread_mutex_lock(&lock_nl);
			num_leitores--;
			if(num_leitores == 0){ 
				pthread_mutex_unlock(&lock_bd);
			}
        pthread_mutex_unlock(&lock_nl);
        use_data_read(i);        /* região não crítica */

		
	}
    pthread_exit(0);
}

// Starvation dos escritores

void * writer(void *arg) {
	int i = *((int *) arg);
	while(TRUE) {               /* repete para sempre */
		think_up_data(i);        /* região não crítica */

		/* Ja passou o processamento do think_up_data entao esta pronto para escrever */
		/* Agora com o lock solução temos certeza que nenhum novo leitor sera criado e tentará acesso ao bd enquando houver escritores */
		/* Sem novos leitores travando o lock_bd não haverá mais starvation dos escritores */
		pthread_mutex_lock(&lock_solucao); 
			pthread_mutex_lock(&lock_bd);
				write_data_base(i);      /* atualiza os dados */
			pthread_mutex_unlock(&lock_bd);
		pthread_mutex_unlock(&lock_solucao);

    }
    pthread_exit(0);
}

void read_data_base(int i) {
	printf("Leitor %d está lendo os dados! Número de leitores: %d\n", i,num_leitores);
	sleep(rand() % 5);
}

void use_data_read(int i) {
	printf("Leitor %d está usando os dados lidos! Número de leitores: %d\n", i,num_leitores);
	sleep(rand() % 5);
}

void think_up_data(int i) {
	printf("Escritor %d está pensando no que escrever!\n", i);
	sleep(rand() % 5);
}

void write_data_base(int i) {
	printf("Escritor %d está escrevendo os dados! Número de leitores: %d\n", i,num_leitores);
	sleep( rand() % 5 + 5);
}
