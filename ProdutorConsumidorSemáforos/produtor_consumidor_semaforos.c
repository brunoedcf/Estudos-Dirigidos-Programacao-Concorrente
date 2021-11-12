#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

//Bruno Esteves 17/0100863

#define PR 2 //número de produtores
#define CN 4 // número de consumidores
#define N 5  //tamanho do buffer

sem_t buffer;
sem_t size;
pthread_mutex_t lock_cont = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_buffer = PTHREAD_MUTEX_INITIALIZER;


int cont = 0;

void * produtor(void * meuid);
void * consumidor (void * meuid);


void main(argc, argv)
int argc;
char *argv[];
{

  // Iniciar o semaforo com a capacidade do buffer de permissoes
  sem_init(&buffer,0,N);
  // Iniciar o semaforo com posiçoes ocupadas do buffer
  sem_init(&size,0,0);

  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[PR];
   
  for (i = 0; i < PR; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, produtor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t tCid[CN];

  for (i = 0; i < CN; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tCid[i], NULL, consumidor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
 
  pthread_join(tid[0],NULL);

}

void * produtor (void* pi)
{
 
  while(1)
  {

    
    printf("Item produzido, esperando espaço no buffer\n");
    sem_wait(&buffer);

    pthread_mutex_lock(&lock_cont);
      cont++;
    pthread_mutex_unlock(&lock_cont);

    sem_post(&size);
   
    printf("Item inserido no buffer\n");
    
    sleep(rand()%20);

  }
  pthread_exit(0);
  
}

void * consumidor (void* pi)
{
 while(1)
  {

    printf("Esperando item para ser consumido\n");
    sem_wait(&size);
  
    pthread_mutex_lock(&lock_cont);
      cont--;
    pthread_mutex_unlock(&lock_cont);

    sem_post(&buffer);

    printf("Item consumido\n");
    sleep(rand()%20);
  
  }
  pthread_exit(0);
  
}
