#include "pipe_networking.h"
/*
This server fork to handle multiple concurrent connections.


To provide some communication, after the handshake:

Modify the client + subserver to allow multiple requests per connection.
Do this by sending a string to the server and get responses every second (use sleep(1) after recieving your message, print the result.)
You can test by spawning multiple clients and verifying they all can maintain communication with the subservers

The end result should be one server that processes client input from any number of clients.

You can spawn 2 or more clients, each of which should be printing data every second.


*/
int server_setup() {
  int from_client = 0;
  // make WKP
  if (mkfifo(WKP, 0644)==-1) perror("making WKP failed");

  // open wkp.[blocks]
  from_client = open(WKP, O_RDONLY);
  if (from_client==-1) err();
  printf("server setup done: created WKP, waiting for client\n");

  // step 3 in client

  // remove the wkp
  remove(WKP);

  return from_client;
}


#include "pipe_networking.h"

// for pipe, look for sigpipe
static void sighandler(int signo){
    if (signo == SIGINT){
        remove(WKP);
        printf("Removed WKP and closed files.");
        exit(0);
    }
    if (signo == SIGPIPE){
        printf("Disconnected.\n");
    }
}

void server_handshake_half(int *to_client, int from_client){

    // read from client and respond to client
    char rec[HANDSHAKE_BUFFER_SIZE];

    int read_rec = read(from_client,rec,HANDSHAKE_BUFFER_SIZE);
    if (read_rec==-1) perror("reading error");
    printf("received from client: %s\n",rec);

    char* send = "returning handshake!"; // ?
    int wr = write(*to_client, send, sizeof(send)+1);
    if (wr==-1) perror("writing error");
    printf("subserver handshake complete!\n");

}

// REMEMBER TO WRITE THE H FILE !!!!!!!!!!
int main() {
    int from_client;
    int to_client;
    pid_t p;

    // for signals
    signal(SIGPIPE, sighandler);
    signal(SIGINT, sighandler);

    while(1){ // loop the steps
        // create the WKP, await connection, upon connection remove the WKP.
        from_client = server_setup(); // handled by server setup

        // Fork to create a subserver which will handle the client.
        p = fork();

        if (p == 0){ // child, handle the client (like basic server)

            // Complete the 3-way handshake.
            server_handshake_half(&to_client, from_client);

            char buff[300];
            // read continuously, receive string from client
            while(read(from_client, buff, sizeof(buff))>0){
                printf("subserver received: %s\n", buff);
                sleep(1);

                // echo string to client
                write(to_client, buff, sizeof(buff)+1);
                printf("server echo to client: %s\n",buff);
            }
            printf("client disconnected.\n");
            close(from_client);
            close(to_client);
            exit(0);

        }
    }
    return 0;
}
