#include "common.h"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <deque>
#include <map>
#include <thread>
#include <mutex>
#include <string>
#include <cstring>

using namespace std;

struct clientMessage {
  int type;
  int socket;
  int pid;
};
 
// Variáveis globais
deque<clientMessage> queue; // fila de mensagens
mutex queueMutex; // mutex da fila para sincronizar o acesso
bool locked = false;
map<int, int> pidListenCount; // map de quantas vezes cada PID foi atendido

void InterfaceThread();
void SocketConnectionsThread();
void HandleNewConnection(int socket);
void handleRequest(clientMessage msg);
void handleRelease(clientMessage msg);
void handleGrant();
clientMessage getMessage(char *buffer, int fd);

int main(int argc, char const *argv[]) {

  // Thread para atender a interface
  thread interface_thread = thread(InterfaceThread);
  
  // Thread para receber conexão de novos processos
  thread socket_thread = thread(SocketConnectionsThread);

  interface_thread.join();
  socket_thread.join();
  
  return 0;
}

void InterfaceThread() {
  cout << "Comandos coordenador:" << endl;
  cout << "1) Imprimir a fila de pedidos atual" << endl;
  cout << "2) Imprimir quantas vezes cada processo foi atendido" << endl;
  cout << "3) Encerrar a execução" << endl;
  string command;
  while (true) {
    cout << "\nComando: ";
    cin >> command;
    if (command == "1") {
      cout << "Fila de pedidos atual:" << endl;
      queueMutex.lock();
      if (queue.empty())
        cout << "Fila vazia!" << endl;
      else
        for (int i = 0; i < queue.size(); i++)
          cout << "  - " << i + 1 << "º: PID " << queue[i].pid << endl;
      queueMutex.unlock();
    } else if (command == "2") {
      cout << "Histórico de processos atendidos:" << endl;
      map<int, int>::iterator it = pidListenCount.begin();
      int key, value;
      for (it; it != pidListenCount.end(); ++it) {
        key = it->first;
        value = it->second;
        cout << " - PID " << key << " = " << value << endl;
      }        
    } else if (command == "3") {
      cout << "Encerrando execução..." << endl;
      exit(EXIT_SUCCESS);
    } else {
      cout << "Comando inválido!" << endl;
    }
  }
}

void SocketConnectionsThread() {
  int Socket, new_socket, optval;
  struct sockaddr_in serverAddr;
  socklen_t addrLen;

  Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // domain, type, protocol
  if (Socket < 0)
    PRINT_ERROR_AND_EXIT("Erro ao abir o socket!");

  optval = 1;
  if (setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval)) < 0)
    PRINT_ERROR_AND_EXIT("Erro ao configurar o socket!");

  serverAddr.sin_family = AF_INET; // IPv4
  serverAddr.sin_port = htons(PORT); // Porta
  serverAddr.sin_addr.s_addr = INADDR_ANY; // Todas as interfaces de rede
  addrLen = sizeof(serverAddr);

  if (bind(Socket, (struct sockaddr *)&serverAddr, addrLen) < 0)
    PRINT_ERROR_AND_EXIT("Erro no bind!");
  if (listen(Socket, 1) < 0)
    PRINT_ERROR_AND_EXIT("Erro no listen!");

  while (true) {
    new_socket = accept(Socket, (struct sockaddr *)&serverAddr, &addrLen);
    if (new_socket < 0)
      PRINT_ERROR_AND_EXIT("Erro no aceite de nova conexão!");
    thread(HandleNewConnection, new_socket).detach();
  }
  return;
}

void HandleNewConnection(int socket) {
  int bytes_read;
  char socket_msg[BUFFER_SIZE];
  clientMessage message;
  while(true) {
    bytes_read = read(socket, &socket_msg, BUFFER_SIZE);

    if (bytes_read == BUFFER_SIZE) { // Recebeu mensagem com tamnaho correto
      message = getMessage(socket_msg, socket);
      if (message.type == REQUEST) {
        if (pidListenCount.find(message.pid) == pidListenCount.end())
          pidListenCount[message.pid] = 0;
        handleRequest(message);
      } else if (message.type == RELEASE) {
        handleRelease(message);
      } else {
        PRINT_ERROR_AND_EXIT("Mensagem inválida!");
      }
    } else if (bytes_read == 0) {  // Cliente fechou o socket
      close(socket);
      return;
    } else {
      PRINT_ERROR_AND_EXIT("Erro na leitura do socket!");
    }
  }
}

clientMessage getMessage(char *buffer, int fd) {
  clientMessage message;
  string aux(buffer);
  int start = 2;
  int end = aux.find_last_of(SEPARATOR);
  message.socket = fd;
  message.type = atoi(&aux[0]);
  message.pid = atoi(aux.substr(start, end-start).c_str());
  return message;
}

void handleGrant() {
  locked = true; // passar pra baixo?
  char socket_msg[BUFFER_SIZE];
  clientMessage message = queue.front();
  queue.pop_front();
  
  string id_s = to_string(message.pid);
  
  string aux(BUFFER_SIZE - 1, '0');
  aux[0] = GRANT + '0'; // int to char
  aux[1] = SEPARATOR;
  aux.replace(2, id_s.length() + 1, id_s + SEPARATOR);
  
  strcpy(socket_msg, aux.c_str());    
  write(message.socket, &socket_msg, BUFFER_SIZE);
  pidListenCount[message.pid]++;
}

void handleRequest(clientMessage msg) {
  queueMutex.lock();
  queue.push_back(msg);
  if (!locked)
    handleGrant();
  queueMutex.unlock(); 
}

void handleRelease(clientMessage msg) {
  queueMutex.lock();
  if (queue.empty())
    locked = false;
  else
    handleGrant();
  queueMutex.unlock();
}

