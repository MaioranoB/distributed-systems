#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

#define PORT 1512
#define BUFFER_SIZE 20

using namespace std;

int main() {
    int howMany, SOCKET;
    struct sockaddr_in serverAddr;
    srand(time(NULL)); // seed

    cout << "Quantos números aleatórios você quer saber se são primos? (Não esqueça de rodar o consumidor)" << endl;
    cin >> howMany;
    cout << endl;

    SOCKET = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // domain, type, protocol
    if (SOCKET < 0) {
        cout << "Erro ao abir o socket." << endl;
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET; // IPv4
    serverAddr.sin_port = htons(PORT); // Porta
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Todas as interfaces de rede

    if (connect(SOCKET, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        cout << "Falha ao se conectar ao socket na porta " << PORT << endl;
        close(SOCKET);
        exit(EXIT_FAILURE);
    }

    int product = 1;
    char socket_msg[BUFFER_SIZE];
    for (; howMany > 0; howMany--) {
        cout << product << " é primo?" << endl;
        sprintf(socket_msg, "%d", product);
        write(SOCKET, &socket_msg, sizeof(char) * BUFFER_SIZE);
        read(SOCKET, &socket_msg, sizeof(char) * BUFFER_SIZE);
        cout << "[CONSUMIDOR]: " << socket_msg << endl;
        product += rand() % 100 + 1;
    }

    sprintf(socket_msg, "%d", 0);
    write(SOCKET, &socket_msg, sizeof(char) * BUFFER_SIZE);
    close(SOCKET);

    return 0;
}