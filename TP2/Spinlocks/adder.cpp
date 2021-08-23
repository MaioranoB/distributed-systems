#include <unistd.h>
#include <iostream>
#include <atomic>
#include <pthread.h>
#include <time.h>

using namespace std;

struct Lock {
  atomic_flag held = ATOMIC_FLAG_INIT;
};

void acquire(Lock *lock) {
  while(lock->held.test_and_set()); // busy wait
}

void release(Lock *lock) {
  lock->held.clear();
}

// Estrutura de argumento que cada thread recebe
struct ThreadArguments {
  Lock *lock;
  char *vetor;
  int *parcela;
  int *SOMA;
};

int RandIntBetween(int min, int max) {
  return rand()%(max-min + 1) + min;
}

void *Somador(void *args){
  ThreadArguments *thisThreadArg = (ThreadArguments *) args;
  int parcela = *thisThreadArg->parcela;
  int somaAUX = 0;

  for (int i=0; i<parcela; i++) {
    somaAUX += (int)thisThreadArg->vetor[i];
  }

  acquire(thisThreadArg->lock);
  *thisThreadArg->SOMA += somaAUX;
  release(thisThreadArg->lock);

  return NULL;
}

int main(int argc, char const *argv[]){
  if (argc != 3) {
    cout << "Passe o tamanho do vetor [N] e o número de threads [K] como parâmetros." << endl;
    exit(EXIT_FAILURE);
  }
  srand(time(NULL)); // seed

  int N = atoi(argv[1]); // tamanho do vetor
  char *vetor = new char[N]; // alocar apenas 1byte para cada número
  
  int K = atoi(argv[2]); // número de threads
  pthread_t threads[K];
  ThreadArguments **arguments = new ThreadArguments *[K];

  int parcelas = N/K;
  int SOMA = 0;
  int i;
  Lock lock;

  // Preencher o vetor de forma aleatória
  for (i=0; i<N; i++) {
    vetor[i] = (char)RandIntBetween(-100,100);
  }
  
  // Criar estrutura de argumentos de cada thread
  for (i=0; i<K; i++){
    arguments[i] = new ThreadArguments();
    arguments[i]->lock = &lock;
    arguments[i]->vetor = &vetor[parcelas*i]; // ponteiro para a respectiva posição do vetor
    arguments[i]->parcela = &parcelas;
    arguments[i]->SOMA = &SOMA;
    
  }

  // Obter média do tempo das 10 execuções
  clock_t start, end;
  double media = 0;
  int repeat = 10;
  for (int z=0; z<repeat; z++) {
    
    // Criar as threads
    start = clock();
    for (i=0; i<K; i++) {
      pthread_create(&threads[i], NULL, Somador, (void *) arguments[i]);
    }
    
    // Esperar todas as threads terminarem
    for (i=0; i<K; i++) {
      pthread_join(threads[i],NULL);
    }
    end = clock();

    media += (double)(end - start)/CLOCKS_PER_SEC;
    SOMA = 0; // zerar SOMA 
  }
  
  cout << N << "," << K << "," << media/(double)repeat << endl;
  return 0;
}
