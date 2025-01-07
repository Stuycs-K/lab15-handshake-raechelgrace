#include "pipe_networking.h"

// for pipe, look for sigpipe
static void sighandler(int signo){
    if (signo == SIGINT){
        printf("Disconnected.\n");
        exit(0);
    }
    if (signo == SIGPIPE){
        printf("Disconnected.\n");
        exit(0);
    }
}

int main() {

  signal(SIGPIPE, sighandler);

  int to_server;
  int from_server;

  from_server = client_handshake( &to_server );

  char buff[300];
  sprintf(buff, "client %d says hello", getpid());

  // read and print int from server
  while(1){ // until ctrl c, so forever
  /*
    int r = read(from_server, buff, sizeof(buff));
    if (r<=0){
      printf("Disconnected,\n");
      break;
    }
    printf("Received %d from server\n", buff);
  }
  */
    int wr = write(to_server, buff, strlen(buff)+1);
    if (wr==-1){
      perror("error: can't write to server");
      break;
    }
    printf("client sent %s\n",buff);
    sleep(1);
  }
}