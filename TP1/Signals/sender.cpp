#include <iostream>
#include <signal.h>

using namespace std;

int main(int argc, char const *argv[]) {
  if (argc != 3) {
    cout << "Passe o número do processo e o sinal como parâmetros: [PID] [signal]" << endl;
    exit(EXIT_FAILURE);
  }

  int PID = atoi(argv[1]);
  int signal = atoi(argv[2]);

  // Verificar se o processo existe
  bool PIDexist = kill(PID,0) == 0;

  if (!PIDexist) {
    cout << "O processo " << PID << " não existe!" << endl;
    exit(EXIT_FAILURE);
  }

  cout << "Enviando para o processo " << PID << " o sinal " << signal << endl;
  int response = kill(PID,signal);
  
  if (response == 0) {
    cout << "Sinal enviado." << endl;
  } else {
    cout << "Erro ao enviar o sinal." << endl;
  }

  return 0;
}
