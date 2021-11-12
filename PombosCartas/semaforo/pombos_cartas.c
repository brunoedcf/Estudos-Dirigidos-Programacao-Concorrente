#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila

void * f_usuario(void *arg);
void * f_pombo(void *arg);

sem_t pombo;  
sem_t cartas;   
sem_t local;
sem_t voar;

int cards = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv){
    
    sem_init(&pombo, 0, 1); //Como se fosse o lock de uso do pombo
	sem_init(&cartas, 0, CARTAS);
    sem_init(&voar, 0, 0);

    int i;

    pthread_t usuario[N];
    int *id;
    for(i = 0; i < N; i++){
         id = (int *) malloc(sizeof(int));
          *id = i;
	pthread_create(&(usuario[i]),NULL,f_usuario,  (void *) (id));
    }
     pthread_t pombo;
     id = (int *) malloc(sizeof(int));
     *id = 0;
     pthread_create(&(pombo),NULL,f_pombo, (void*) (id));

     pthread_join(pombo,NULL);
   
}


void * f_pombo(void *arg){

    int j;
  
    while(1){
        sem_wait(&voar);
        sem_wait(&pombo);
        printf("Viajando para B\n");
        sleep(2);
        printf("Chegou, entregando cartas\n");
        cards = 0;
        for(j = 0;j < CARTAS; j++){
            sem_post(&cartas);
        }
        sleep(2);
        printf("Viajando para A\n");
        sleep(2);
        printf("Chegou em A\n");
        sem_post(&pombo);
        //Inicialmente está em A, aguardar/dorme a mochila ficar cheia (20 cartas)
        //Leva as cartas para B e volta para A
        //Acordar os usuários   
    }
}

void * f_usuario(void *arg){

    int n = *((int *) arg);
    
    while(1){
	//Escreve uma carta

    //usuarios podem escrever cartas mesmo se o pombo nao estiver presente
    printf("Pessoa %d screvendo carta\n", n);
    sleep(1);
    printf("Carta da pessoa %d escrita\n", n);
    sem_wait(&cartas); //ver se bolsa esta cheia
    sem_wait(&pombo); //ver se ninguem esta colocando nada no pombo ou ele esta em outro lugar

        pthread_mutex_lock(&lock); //talvez eu nao precise desse lock
            cards++;
            printf("Carta da pessoa %d colocada na mochila, total = %d\n", n, cards);
        pthread_mutex_unlock(&lock);

    if(cards == CARTAS){
        printf("Mochila cheia, pombo vai voar!\n");
        sem_post(&voar); //acorda o pombo
    }

    sem_post(&pombo); //libera o pombo

        //Caso o pombo não esteja em A ou a mochila estiver cheia, então dorme	
        //Posta sua carta na mochila do pombo
        //Caso a mochila fique cheia, acorda o ṕombo
    }
}
