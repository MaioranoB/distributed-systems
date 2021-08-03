#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <math.h>

#define PORT 1512

using namespace std;

int main() {
    int howMany, recieved;
    cout << "Quantos números aleatórios você quer saber se são primos? (Não esqueça de rodar o consumidor)" << endl;
    cin >> howMany;
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Erro criando o socket." << endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Erro ao conectar no socket do consumidor.");
        return -1;
    }

    int start_n = 1;
    char buffer[30];
    for (; howMany > 0; howMany--) {
        start_n = start_n + rand() % 100 + 1;
        sprintf(buffer, "%d", start_n);
        cout << start_n << " é primo?" << endl;
        send(sock, buffer, sizeof(char) * 30, 0);
        recieved = read(sock, buffer, 30);
        cout << "[CONSUMIDOR]: " << buffer << endl;
    }

    sprintf(buffer, "%d", 0);
    send(sock, buffer, sizeof(char) * 30, 0);
    return 0;
}