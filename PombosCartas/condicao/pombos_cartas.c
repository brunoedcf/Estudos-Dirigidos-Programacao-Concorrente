#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t pombo = PTHREAD_COND_INITIALIZER;
pthread_cond_t escritores = PTHREAD_COND_INITIALIZER;

int cartas = 0;
int local = 0;

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
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
  
    while(1){
        //Inicialmente está em A, aguardar/dorme a mochila ficar cheia (20 cartas)
        pthread_mutex_lock(&lock);

            while(cartas < CARTAS){ //enquanto o saco não esta cheio
                pthread_cond_wait(&pombo, &lock);
            }
            printf("Levando cartas para B\n");
            sleep(2);
            printf("Entregando cartas!\n");
            local = 1;
            cartas = 0;
            sleep(2);
            printf("Voltando para A\n");
            sleep(2);
            printf("Pombo retornou\n");
            local = 0;
        //Leva as cartas para B e volta para A
        pthread_mutex_unlock(&lock); 

        pthread_cond_broadcast(&escritores);
        //Acordar os usuários  
    }
}

void * f_usuario(void *arg){
    
    while(1){
        pthread_mutex_lock(&lock);

            while(cartas == CARTAS || local == 1){
                pthread_cond_wait(&escritores, &lock);
            }
            printf("Escrevendo carta\n");
            cartas++;
            printf("Carta escrita: %d\n", cartas);

            if(cartas == CARTAS){
                printf("Mochila cheia!\n");
                pthread_cond_signal(&pombo);
            }

        pthread_mutex_unlock(&lock);
	//Escreve uma carta
        //Caso o pombo não esteja em A ou a mochila estiver cheia, então dorme	
        //Posta sua carta na mochila do pombo
        //Caso a mochila fique cheia, acorda o ṕombo
    }
}
