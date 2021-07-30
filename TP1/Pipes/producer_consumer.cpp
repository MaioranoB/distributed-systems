#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <math.h>

using namespace std;

#define BUFFER_SIZE 20

bool ehPrimo(int x){
  for (int i=2; i<=sqrt(x); i++) {
    if (x%i == 0) {
      return false;
    }
  }
  return true;
}

int randomDelta(void) {
  return rand() % 100 + 1;
}

void Produtor(int PIPE[], int n) {
  char pipe_msg[BUFFER_SIZE];
  int product = 1;
  
  while (n) {
    cout << "[PRODUTOR] escrevendo " << product << " no pipe" << endl;
    sprintf(pipe_msg, "%d", product);
    write(PIPE[1], &pipe_msg, sizeof(char)*BUFFER_SIZE); // escreve na write end
    product += randomDelta();
    n--;
  }

  // Sinalizar que finalizou enviando 0
  sprintf(pipe_msg, "%d", 0);
  write(PIPE[1], &pipe_msg, sizeof(char)*BUFFER_SIZE);
  cout << endl;
}

void Consumidor(int PIPE[]) {
  char pipe_msg[BUFFER_SIZE];
  int received;

  while (1) {
    read(PIPE[0], &pipe_msg, sizeof(char)*BUFFER_SIZE); // le da read end
    received = atoi(pipe_msg);

    if(received == 0){
      return;
    } else if (ehPrimo(received)){
      cout << "[CONSUMIDOR] " << received << " é primo." << endl;
    } else {
      cout << "[CONSUMIDOR] " << received << " não é primo." << endl;
    }
  }
}

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    cout << "Passe a quantidade de números a serem gerados pelo produtor como parâmetro." << endl;
    exit(EXIT_FAILURE);
  }

  int n = atoi(argv[1]);
  srand(time(NULL)); // seed

  int PIPE[2];
  if (pipe(PIPE) != 0) {
    cout << "Erro ao instanciar o pipe." << endl;
    exit(EXIT_FAILURE);
  }

  pid_t child_pid = fork();
  if (child_pid == 0) {
    Consumidor(PIPE); // processo filho
  } else {
    Produtor(PIPE,n); // processo pai
  }

  return 0;
}
