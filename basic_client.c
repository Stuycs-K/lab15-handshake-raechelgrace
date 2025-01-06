#include "pipe_networking.h"

int main() {

  int to_server;
  int from_server;
  int x;

  from_server = client_handshake( &to_server );

  // read and print int from server
  while(1){ // until ctrl c, so forever
    int r = read(from_server, &x, sizeof(x));
    if (r<=0){
      printf("Disconnected,\n");
      break;
    }
    printf("Received %d from client\n", x);
  }
}
