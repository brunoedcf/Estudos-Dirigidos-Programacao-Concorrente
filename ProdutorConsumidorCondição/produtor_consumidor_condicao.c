#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Bruno Esteves 17/0100863

#define PR 10 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer


void * produtor(void * meuid);
void * consumidor (void * meuid);

int cont = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t condConsumidor = PTHREAD_COND_INITIALIZER;
pthread_cond_t condProdutor = PTHREAD_COND_INITIALIZER;


void main(argc, argv)
int argc;
char *argv[];
{

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
    // gera um item
    // checa se buffer esta cheio
      // sleep
    // insere no buffer e incremento contador
    // checa se estava vazio o buffer
      // acorda consumidor

    printf("Produtor: Produzindo item\n");
    pthread_mutex_lock(&lock);

      sleep(2);
    
      while(cont == N){
        printf("Produtor: Esperando espaço no buffer\n");
        pthread_cond_wait(&condProdutor, &lock);
      }

      cont++;
      printf("Produtor: Inserindo no buffer. Total = %d\n", cont);
        
      if(cont == 1){
        pthread_cond_signal(&condConsumidor);
        printf("Produtor: Acordando consumidor\n");
      }

    pthread_mutex_unlock(&lock);


  }
  pthread_exit(0);
  
}

void * consumidor (void* pi)
{
 while(1)
  {
    // checa se há algo pra consumir
      // se não, sleep
    // consome o item e decrementa contador
    // checa se estava cheio o buffer
      // acorda produtor


    pthread_mutex_lock(&lock);
      sleep(5);

      while(cont == 0){
        printf("Consumidor: Esperando algum item no buffer\n");
        pthread_cond_wait(&condConsumidor, &lock);
      }

      cont--;
      printf("Consumidor: Consumindo do buffer. Total = %d\n", cont);

      if(cont == N-1){
        pthread_cond_signal(&condProdutor);
        printf("Consumidor: Acordando produtor\n");
      }

    pthread_mutex_unlock(&lock);    
  }
  pthread_exit(0);
  
}
