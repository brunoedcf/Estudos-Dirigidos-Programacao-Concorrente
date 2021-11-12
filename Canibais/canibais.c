#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* Professor, por algum motivo sempre em alguma execução algum canibal nunca consegue
comer a comida, não consegui resolver esse problema */

// Bruno Esteves 17/0100863

#define MAXCANIBAIS 20

int cont = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockCont = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t condConsumidor = PTHREAD_COND_INITIALIZER;
pthread_cond_t condProdutor = PTHREAD_COND_INITIALIZER;


void *canibal(void*meuid);
void *cozinheiro(int m);


void main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[MAXCANIBAIS];

  if(argc != 3){
    printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
    exit(1);
  }
  
  n = atoi (argv[1]); //número de canibais
  m = atoi (argv[2]); // quantidade de porções que o cozinheiro consegue preparar por vez
  printf("numero de canibais: %d -- quantidade de comida: %d\n", n, m);

  if(n > MAXCANIBAIS){
    printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
    exit(1);
  }
  
  for (i = 0; i < n; i++)  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, canibal, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  cozinheiro(m);
}

void * canibal (void* pi){
  
  while(1) {
    //pegar uma porção de comida e acordar o cozinheiro se as porções acabaram

    //checar se não ha ninguem usando o caldeirao
    pthread_mutex_lock(&lock);
      while(cont == 0){
        printf("Canibal %d: Esperando novas porções\n", *(int *)(pi));
        // acordar o cozinheiro e dormir
        printf("Canibal %d: Chamando cozinheiro e indo dormir\n", *(int *)(pi));
        pthread_cond_signal(&condProdutor);
        pthread_cond_wait(&condConsumidor, &lock);
      }
      pthread_mutex_lock(&lockCont);
        cont--;
      pthread_mutex_unlock(&lockCont);

    pthread_mutex_unlock(&lock);
    printf("%d: vou comer a porcao que peguei. Restam %d\n", *(int *)(pi), cont);
    
    sleep(10);
  }
  
}

void *cozinheiro (int m){
 
  while(1){
     
    pthread_mutex_lock(&lock);
      //dormir enquanto tiver comida

      while(cont > 0){
        printf("Cozinheiro dormindo!\n");
        pthread_cond_wait(&condProdutor, &lock);
      }

      cont = m;
      printf("Cozinhando %d porções\n", m);
      sleep(10);
      //acordar os canibais
      // como tenho que acordar todos ao mesmo tempo, utilizo o broadcast
      if(cont == m) pthread_cond_broadcast(&condConsumidor);

    pthread_mutex_unlock(&lock);
   }
   

}
