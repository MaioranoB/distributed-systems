#include <iostream>

#define PORT 1512
#define BUFFER_SIZE 10
#define SEPARATOR '|'

// Message types
#define REQUEST 1
#define GRANT 2
#define RELEASE 3

void PRINT_ERROR_AND_EXIT(const char* error){
  std::cout << error << std::endl;
  exit(EXIT_FAILURE);
}
