#include "pipe_networking.h"

/*
choose a random int x from 0 to 100 and then send it to the client
once every second (sleep after sending)

client should reapatedly read the int sent by the server and print it

If you ctrl-c the client or the server they should both exit gracefully.
*/

int main() {
  int to_client;
  int from_client;
  srand(getpid());
  int x;

  from_client = server_handshake( &to_client );

  while(1){ // until ctrl c, so forever
    x = rand()%(100+1); // random number in the range [0,100]
    write(to_client, &x, sizeof(x));
    printf("Sent %d to client\n", x);
    sleep(1);
  }
  printf("Disconnected.\n");
}
