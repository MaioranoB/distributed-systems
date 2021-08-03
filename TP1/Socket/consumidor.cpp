#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>

#define PORT 1512
#define BUFFER_SIZE 20

using namespace std;

bool ehPrimo(int x) {
  for (int i=2; i<=sqrt(x); i++) {
    if (x%i == 0) {
      return false;
    }
  }
  return true;
}

int main() {
    int SOCKET, novoSocket;
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    SOCKET = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Tratando os erros
    if (SOCKET < 0) {
        cout << "Erro ao abrir o socket" << endl;
        exit(EXIT_FAILURE);
    }
    if (bind(SOCKET, (struct sockaddr *)&addr, addrLen) < 0) {
        cout << "Erro no bind" << endl;
        exit(EXIT_FAILURE);
    }
    if (listen(SOCKET, 1) < 0) {
        cout << "Erro no listen" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Consumidor escutando na porta " << PORT << endl;
    novoSocket = accept(SOCKET, (struct sockaddr *)&addr, &addrLen);
    if (novoSocket < 0) {
        cout << "Erro no aceite de nova conexão" << endl;
        exit(EXIT_FAILURE);
    }
    
    char socket_msg[BUFFER_SIZE];
    int received;
    while (true) {
        read(novoSocket, &socket_msg, sizeof(char) * BUFFER_SIZE);
        received = atoi(socket_msg);
        if (received == 0)
            break;
        cout << "Recebi " << received << endl;
        sprintf(socket_msg, "%d %s.", received, (ehPrimo(received) ? "é" : "não é"));
        write(novoSocket, &socket_msg, sizeof(char) * BUFFER_SIZE);
    }

    close(SOCKET);
    return 0;
}