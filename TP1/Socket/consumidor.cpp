#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <math.h>

#define PORT 1512

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
    char buffer[20];
    int produtorId, novoSocket, recieved;
    struct sockaddr_in addr;
    int addrSize = sizeof(addr);
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    // Tratando os erros
    if ((produtorId = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        cout << "Erro criando o socket" << endl;
        return 1;
    }
    if (bind(produtorId, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        cout << "Erro no bind" << endl;
        return 1;
    }
    if (listen(produtorId, 1) < 0) {
        cout << "Erro no listen" << endl;
        return 1;
    }

    cout << "Consumidor escutando na porta " << PORT << endl;
    novoSocket = accept(produtorId, (struct sockaddr *)&addr, (socklen_t *)&addrSize);
    if (novoSocket < 0) {
        cout << "Erro no aceite de nova conexão" << endl;
        return 1;
    }

    while (true) {
        recieved = read(novoSocket, buffer, sizeof(char) * 30);
        int number = atoi(buffer);
        if (number == 0)
            break;
        cout << "Recebi " << number << endl;
        char response[30];
        sprintf(response, "%i%s", number, (ehPrimo(number) ? " é." : " não é."));
        send(novoSocket, response, sizeof(char) * 30, 0);
    }

    close(produtorId);
    return 0;
}