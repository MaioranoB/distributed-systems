#include <iostream>
#include <signal.h>
#include <unistd.h>

using namespace std;

void signal_handler(int signal) {
  switch (signal) {
    case SIGHUP:
      cout << "SIGHUP recebido" << endl;
      break;
    case SIGINT:
      cout << "SIGINT recebido" << endl;
      break;
    case SIGQUIT:
      cout << "SIGQUIT recebido. Terminando o processo..." << endl;
      exit(EXIT_SUCCESS);
      break;
  }
}

int main(int argc, char const *argv[]) {
  pid_t receiverPID = getpid();
  cout << "Executando a receiver... [PID] = " << receiverPID << endl;

  int wait_type = 2;
  cout << "Digite 0 para busy wait ou 1 para blocking wait: ";
  cin >> wait_type;
  
  while (wait_type!=0 && wait_type!=1) {
    cout << "Digite 0 para busy wait ou 1 para blocking wait: ";
    cin >> wait_type;
  }

  signal(SIGHUP, signal_handler);
  signal(SIGINT, signal_handler);
  signal(SIGQUIT, signal_handler);

  cout << "Sinais implementados e seus respectivos códigos:" << endl;
  cout << "SIGHUP  = " << SIGHUP << endl;
  cout << "SIGINT  = " << SIGINT << endl;
  cout << "SIGQUIT = " << SIGQUIT << " [EXIT]" << endl;

  if (wait_type == 0) {
    while (1); // busy wait
  } else {
    while(1) { pause(); } // blocking wait
  }

  return 0;
}
