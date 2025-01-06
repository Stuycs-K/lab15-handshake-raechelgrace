/*
This server fork to handle multiple concurrent connections.

Fork to create a subserver which will handle the client.
Go back to step 1.
A subserver should:

Complete the 3-way handshake.
Handle all commucations with the server.
Notice the server steps 1-3 are all contained in the server setup command! Your server sets up the handshake, but your subserver actually does the rest of the handshake.

Your server will server_setup() and your sub-server will finish the job with server_handshake_half()

To provide some communication, after the handshake:

Modify the client + subserver to allow multiple requests per connection.
Do this by sending a string to the server and get responses every second (use sleep(1) after recieving your message, print the result.)
You can test by spawning multiple clients and verifying they all can maintain communication with the subservers

The end result should be one server that processes client input from any number of clients.

You can spawn 2 or more clients, each of which should be printing data every second.


*/

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

server_handshake_half(int *to_client, int from_client){
    // read from client and respond to client
    char rec[HANDSHAKE_BUFFER_SIZE];
    int read_rec = read(from_client,rec,HANDSHAKE_BUFFER_SIZE);
    if (read_rec==-1) err();
    printf("received from client: %s\n",rec);

    char* send = "hi back"; // ?
    int wr = write(*to_client, send, sizeof(send));
    if (wr==-1) err();

}

// REMEMBER TO WRITE THE H FILE !!!!!!!!!!
int main() {
    int from_client;
    int to_client;

    // for signals
    signal(SIGPIPE, sighandler);
    signal(SIGINT, sighandler);

    srand(getpid());
    int x;

    // create the WKP
    if (mkfifo(WKP, 0644)==-1) err();

    // upon connection remove the WKP.
    remove(WKP);

    // Fork to create a subserver which will handle the client.
    // stuff... and things...


    while(1){ // until ctrl c, so forever

        // inside now, to handle continous client connections
        from_client = server_handshake( &to_client );
    
        // for each client
        while(1){
            x = rand()%(100+1); // random number in the range [0,100]
            if(write(to_client, &x, sizeof(x)) == -1) break;
            printf("Sent %d to client\n", x);
            sleep(1);
        }
        close(from_client);
        close(to_client);
        printf("Disconnected.\n");
    }
}
