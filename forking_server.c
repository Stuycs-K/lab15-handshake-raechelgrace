#include "pipe_networking.h"
/*
This server forks to handle multiple concurrent connections.
*/

#include "pipe_networking.h"

// for pipe, look for sigpipe
static void sighandler(int signo){
    if (signo == SIGINT){
        remove(WKP);
        exit(0);
    }
    if (signo == SIGPIPE){
        printf("Disconnected.\n");
    }
}

void server_handshake_half(int *to_client, int from_client){

    // read from client
    char pp[HANDSHAKE_BUFFER_SIZE];
    int read_rec = read(from_client,pp,HANDSHAKE_BUFFER_SIZE);
    if (read_rec==-1) perror("reading error");
    printf("received from client: %s\n",pp);

    // open private pipe
    *to_client = open(pp, O_WRONLY);
    if (*to_client==-1) perror("private pipe opening didnt work");

    char* send = "returning handshake!\n"; // ?
    int wr = write(*to_client, send, strlen(send)+1);
    if (wr==-1) perror("writing error");
    printf("subserver %d handshake complete!\n", getpid());

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
            while(1){
                int read_client = read(from_client, buff, sizeof(buff));
                if (read_client<=0){
                    printf("client disconnected.\n");
                    close(from_client);
                    close(to_client);
                    exit(0);
                }
                printf("subserver received: %s\n", buff);

                // send message back to client !!
                int w = write(to_client, buff, strlen(buff)+1);
                if (w==-1){
                    break;
                }
                printf("server echoed back to to client: %s\n",buff);

                sleep(1);
            }
        }
        else{ // parent process!
            close(from_client);
        }
    }
    close(from_client);
    close(to_client);
    return 0;
    
}

// stopping doesnt work... breaks the server
