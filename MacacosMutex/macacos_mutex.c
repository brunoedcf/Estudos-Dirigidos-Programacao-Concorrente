#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

//Bruno Esteves 17/0100863

pthread_mutex_t lock_corda = PTHREAD_MUTEX_INITIALIZER;     
pthread_mutex_t lock_vez = PTHREAD_MUTEX_INITIALIZER;       
pthread_mutex_t lock_ApB = PTHREAD_MUTEX_INITIALIZER;       
pthread_mutex_t lock_BpA = PTHREAD_MUTEX_INITIALIZER;      
int ultimo = -1; /*Quem foi o ultimo a passar 0 pra A 1 pra B*/

int A = 0;
int B = 0;


void * macacoAB(void * a) {
  int i = *((int *) a);    
  while(1){

    //Procedimentos para acessar a corda
    //Similar ao problema dos escritores e leitores
    //o lock_vez eh análogo ao lock_solucao do problema passado
    //lock_vez indica que a vez eh dos macacos de A para B passarem
    //quando o primeiro macaco passa, travamos o lock da corda, agora so macacos de A podem passar
    //o lock ApB eh para controle da variavel compartilhada A

    pthread_mutex_lock(&lock_vez); // Resolve o problema de starvation
      if(ultimo != 0) ultimo = 0;
      pthread_mutex_lock(&lock_ApB);
        A++;
        if(A == 1){
          pthread_mutex_lock(&lock_corda);
        }
    pthread_mutex_unlock(&lock_vez);
    pthread_mutex_unlock(&lock_ApB); 
    
    printf("Macaco %d passado de A para B \n",i);   
    sleep(1);

    //Procedimentos para quando sair da corda
    //Mesma coisa da questao anterior, quando nao ha mais macacos, ou leitores, para passar na corda o lock eh liberado

    pthread_mutex_lock(&lock_ApB);            
      A--;                            
      if(A == 0){                    
        pthread_mutex_unlock(&lock_corda);  
      }
    pthread_mutex_unlock(&lock_ApB);

  }
  pthread_exit(0);
}

void * macacoBA(void * a) {
  int i = *((int *) a);    
  while(1){

    //Procedimentos para acessar a corda
    //Copiei e colei a funcao macacoAB e mudei as variaveis
    //Mesma solução

    pthread_mutex_lock(&lock_vez);
      if(ultimo != 1) ultimo = 1;
      pthread_mutex_lock(&lock_BpA);
        B++;
        if(B == 1){
          pthread_mutex_lock(&lock_corda);
        }
      pthread_mutex_unlock(&lock_vez);
    pthread_mutex_unlock(&lock_BpA); 

    printf("Macaco %d passado de B para A \n",i);
    sleep(1);
    //Procedimentos para quando sair da corda

    pthread_mutex_lock(&lock_BpA);            
      B--;                            
      if(B == 0){                    
        pthread_mutex_unlock(&lock_corda);  
      }
    pthread_mutex_unlock(&lock_BpA);

  }
  pthread_exit(0);
}



void * gorila(void * a){
  while(1){
    //Procedimentos para acessar a corda
    //Agora eh a vez do gorila, travamos o lock_vez e da corda
    

    pthread_mutex_lock(&lock_vez);      
      if(ultimo != 2) ultimo = 2;
      pthread_mutex_lock(&lock_corda);  
    pthread_mutex_unlock(&lock_vez);    

    printf("Gorila passado de A para B \n");
    sleep(5);
    //Procedimentos para quando sair da corda
    pthread_mutex_unlock(&lock_corda);   // aqui garantimos a exclusao mutua da corda entre gorilas e macacos
  }
  pthread_exit(0);
}

int main(int argc, char * argv[])
{
    pthread_t macacos[MA+MB];
    int *id;
    int i = 0;

    for(i = 0; i < MA+MB; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        if(i%2 == 0){
          if(pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }else{
          if(pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }
    }
    pthread_t g;
    pthread_create(&g, NULL, &gorila, NULL);

  
    pthread_join(macacos[0], NULL);
    return 0;
}
