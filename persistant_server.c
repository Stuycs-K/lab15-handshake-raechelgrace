/*
server loops to accept a new client after first client disconnects (ctrl-c)

Close the old file descriptors before you end your loop (close from_client and to_client)

You must now also add a sighandler for SIGINT. This will delete the WKP befor ending the server.
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

int main() {
    int from_client;
    int to_client;

    // for signals
    signal(SIGPIPE, sighandler);
    signal(SIGINT, sighandler);

    srand(getpid());
    int x;


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
