#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5
#define ESQ(id) (id+N-1)%N
#define DIR(id)	(id+1)%N

//vetor de estados
int estados[N];

sem_t talher[N];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *filosofos (void *arg);

void com_fome (int n);
void devolve_talher (int n);

int main () {
	int i;
	int *id;
	//todos começam pensandos
	for(i=0; i<N; i++){
		estados[i] = 0;
	}
	//semaforo 
	for (i=0; i<N; i++) {
		//inicializar com 0 para indicar que o talher não está sendo usado
		sem_init(&talher[i], 0, 0);
	}
	pthread_t r[N]; 

	//criacao das threads de filosofos
       for (i = 0; i < N ; i++) {
	     id = (int *) malloc(sizeof(int));
	     *id = i;
	     pthread_create(&r[i], NULL, filosofos, (void*) (id));
	}

        pthread_join(r[0],NULL);
	return 0;
}


void *filosofos (void *arg) {
	int n = *((int *) arg);
	while(1) {
		//pensar
		printf("Filosofo %d pensando ...\n", n);
		sleep(3);
		com_fome(n);
		//comer
		printf("\tFilosofo %d comendo ...\n", n);
		sleep(3);
                printf("\tFilosofo %d acabou de comer ...\n", n);
		devolve_talher(n);
	} 
}

void com_fome (int n) {
    
	//bateu a fome
	pthread_mutex_lock(&lock);	
		estados[n] = 1;

		//testar se pode comer
		//se ninguem do lado está comendo e se está com fome
		//2 é o estado em que se está comendo
		if(estados[ESQ(n)] != 2 && estados[DIR(n)] != 2 && estados[n] == 1){
			//posso comer
			estados[n] = 2;
			//estou comendo
			//estou usando o talher n
			sem_post(&talher[n]);
		}

	pthread_mutex_unlock(&lock);

	//acabei de comer e liberei o lock

	sem_wait(&talher[n]);

}

void devolve_talher (int n) {

	pthread_mutex_lock(&lock);	
        
		//voltar a pensar
		estados[n] = 0;

		//apos devolver o talher, para manter o dinamismo testamos se os vizinhos estão com fome

		//testar o da esquerda
		if(estados[ESQ(ESQ(n))] != 2 && estados[DIR(ESQ(n))] != 2 && estados[ESQ(n)] == 1){
			//posso comer
			estados[ESQ(n)] = 2;
			//estou usando o talher n
			sem_post(&talher[ESQ(n)]);
		}
		
		//testar o da direita
		if(estados[ESQ(DIR(n))] != 2 && estados[DIR(DIR(n))] != 2 && estados[DIR(n)] == 1){
			//posso comer
			estados[DIR(n)] = 2;
			//estou usando o talher n
			sem_post(&talher[n]);
		}

	pthread_mutex_unlock(&lock);	
}

