#include <iostream>
#include <unistd.h>
#include <thread> 
#include <mutex> 
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <vector>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#define M 10000
#define TIMEOUT 200

using namespace std;

vector<int> memoria; 
vector<int> processados; 

// semáforos
mutex s_mutex;
condition_variable s_full, s_empty;

int N; 
int NProd, NCons;
atomic<int> produtoresExecutando(0);
int elementos = 0, produzidos = 0, consumidos = 0; 

int achaEspacoLivre(vector<int> dados) {
    for(int i = 0; i < dados.size(); i++) {
        if(dados[i] == 0) {
            return i;
        } 
    }
    return -1;
}

int achaEspacoOcupado(vector<int> dados) {
    for(int i = 0; i < dados.size(); i++) {
        if(dados[i] != 0) {
            return i;
        }
    }
    return -1;
}

bool ehPrimo(int x) {
  for (int i = 2; i <= sqrt(x); i++) {
    if (x % i == 0) {
      return false;
    }
  }
  return true;
}

void produzir() {
    unique_lock<mutex> lock(s_mutex);
    int pos = achaEspacoLivre(memoria);
    if(pos != -1 && s_full.wait_for(lock, chrono::milliseconds(TIMEOUT), [] { return elementos != N; })) {
        int n = rand() % 10000000 + 1;
        memoria[pos] = n;
        elementos++;
        produzidos++;
        s_empty.notify_all();
    }
}
 
void consumir() { 
    unique_lock<mutex> lock(s_mutex);
    int pos = achaEspacoOcupado(memoria);
    if(pos != -1 && s_empty.wait_for(lock, chrono::milliseconds(TIMEOUT), [] { return elementos > 0; })) {
        // cout << memoria[pos] << (ehPrimo(memoria[pos]) ? " é primo" : " não é primo") << endl;
        processados.push_back(memoria[pos]);
        memoria[pos] = 0;
        elementos--;
        consumidos++;
        s_full.notify_all();
    }
}

void produtor() {
    produtoresExecutando++;
    while(produzidos < M) {
        produzir();
    }
    produtoresExecutando--;
}

void consumidor() {
    while(produtoresExecutando == 0) {
        this_thread::yield();
    }
    while(consumidos < M) {
        consumir();
    }
}


int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Informe ao menos 2 parâmetros: Tamanho, (Número de Threads do Produtor e Número de Threads do Consumidor)" << endl;
        return 0;
    }
    srand(time(NULL));

    N = atoi(argv[1]);
    NProd = atoi(argv[2]);
    NCons = atoi(argv[3]);

    double media = 0;
    int repeat = 10;

        chrono::time_point<std::chrono::system_clock> start, end;
        start = chrono::system_clock::now();
   
        int total = NProd + NCons;
        thread myThreads[total];
            
        for(int i = 0; i < N; i++) {
            memoria.push_back(0);
        }
    
        for(int i = 0; i < NProd; i++){
            myThreads[i] = thread(produtor);
        }
        for(int i = NProd; i < total; i++){
            myThreads[i] = thread(consumidor);
        }

        for (int i = 0; i < total; i++){ 
            myThreads[i].join();
        }

        end = chrono::system_clock::now();
        long elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds> (end-start).count();
        media += (double)(elapsed_ms)/CLOCKS_PER_SEC;

	cout << N <<","<< NProd << "," << NCons <<","<< media/(double)repeat << endl;
    return 0;
}