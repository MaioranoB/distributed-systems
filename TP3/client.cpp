#include "common.h"
#include <iomanip>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <chrono>
#include <string>
#include <cstring>

using namespace std;

int openSocketFD();
void request(int socket, int id);
void release(int socket, int id);

int getTimeStamp() {
  chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
  return ms.count();
}

int main(int argc, char const *argv[]) {
  int n = atoi(argv[1]); // n processos
  int r = atoi(argv[2]); // r repetições
  int k = atoi(argv[3]); // k segundos

  int Socket, timeStamp;
  // pid_t real_pid;
  
  ofstream file;
  file.open("resultado.txt", ios::trunc);
  file << "timeStamp   ,pid" << endl;
  file.close();

  for (int pid = 1; pid <= n; pid++) {
    if (fork() == 0) { // processo filho
      // real_pid = getpid();
      Socket = openSocketFD();
      for (int i = 0; i < r; i++) {
        request(Socket, pid);
        timeStamp = getTimeStamp();
        file.open("resultado.txt", ios::app); // abrir o arquivo em modo append
        file << timeStamp << "," << pid << endl;
        file.close();
        sleep(k);
        release(Socket, pid);
      }
      close(Socket);
      exit(EXIT_SUCCESS);
    }

  }
  
  while (wait(NULL) > 0); // esperar todos os processos filhos
  cout << "*** FIM ***" << endl;
  return 0;
}

int openSocketFD() {
  int Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // domain, type, protocol
  if (Socket < 0)
    PRINT_ERROR_AND_EXIT("Erro ao abir o socket!");

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET; // IPv4
  serverAddr.sin_port = htons(PORT); // Porta
  serverAddr.sin_addr.s_addr = INADDR_ANY; // Todas as interfaces de rede

  if (connect(Socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    PRINT_ERROR_AND_EXIT("Falha ao se conectar ao socket!");
    close(Socket);
  }
  return Socket;
}

void request(int socket, int id) {
  string id_s = to_string(id);
  
  string msg(BUFFER_SIZE - 1, '0');
  msg[0] = REQUEST + '0'; // int to char;
  msg[1] = SEPARATOR;
  msg.replace(2, id_s.length() + 1, id_s + SEPARATOR);

  char socket_msg[BUFFER_SIZE];
  strcpy(socket_msg, msg.c_str());

  // cout << "[request] msg enviada: "<< socket_msg <<endl;
  write(socket, &socket_msg, BUFFER_SIZE);
  read(socket, &socket_msg, BUFFER_SIZE);
  // cout << "[request] msg recebida: "<< socket_msg <<endl;

  if (socket_msg[0] != GRANT + '0') {
    PRINT_ERROR_AND_EXIT("Erro na mensagem recebida!");
    close(socket);
  }
  return;
}

void release(int socket, int id) {
  string id_s = to_string(id);
  
  string msg(BUFFER_SIZE - 1, '0');
  msg[0] = RELEASE + '0';
  msg[1] = SEPARATOR;
  msg.replace(2, id_s.length() + 1, id_s + SEPARATOR);

  char socket_msg[BUFFER_SIZE];
  strcpy(socket_msg, msg.c_str());
  // cout << "[release] msg enviada: "<< socket_msg <<endl;
  write(socket, &socket_msg, BUFFER_SIZE);
}
