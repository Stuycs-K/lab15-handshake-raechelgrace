#include "pipe_networking.h"

//prints errno
int err(){
  printf("Error %d: %s\n", errno, strerror(errno));
  exit(1);
}

// UPSTREAM = to the server / from the client
// DOWNSTREAM = to the client / from the server
/*=========================
  server_setup

  creates the WKP and opens it, waiting for a  connection.
  removes the WKP once a connection has been made

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
  int from_client = 0;
  // make WKP
  if (mkfifo(WKP, 0644)==-1) err();

  // open wkp.[blocks]
  from_client = open(WKP, O_RDONLY); // 0644 or no?
  if (from_client==-1) err();
  printf("server setup done: created WKP, waiting for client\n");

  // step 3 in client

  // remove the wkp
  remove(WKP); // here??x

  return from_client;
}

/*=========================
  server_handshake 
  args: int * to_client

  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe (Client's private pipe).

  returns the file descriptor for the upstream pipe (see server setup).
  =========================*/
int server_handshake(int *to_client) { 
  int from_client;

  // calls server_setup();
  from_client = server_setup();

  // server reading SYN (pid) from the client (match w step 3)
  char pid[HANDSHAKE_BUFFER_SIZE];
  int read_pid = read(from_client,pid,HANDSHAKE_BUFFER_SIZE);
  if (read_pid==-1) err();
  printf("received private pipe name (the pid/syn): %s\n",pid);

  // server opening private pipe [unblock client]
  *to_client = open(pid,  O_WRONLY);
  if (*to_client==-1) err();
  printf("opened private pipe %s\n",pid);

  // server send syn_ack
  int syn_ack = 134340; // just a random num to send to pp
  int w = write(*to_client, &syn_ack, sizeof(syn_ack));
  if (w==-1) err();

  // step 8 in client

  // server read ack from client (step 8), handshake complete
  int ack;
  int r = read(from_client, &ack, sizeof(ack)); // read from client to ack
  if (r==-1) err();
  printf("server received final ack %d\n",ack);

  // end server_handshake()
  printf("%d should equal %d + 1\n", ack, syn_ack);

  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) { // im confused... do i use to_server or to_client for wkp and pp? :P to_server = WKP...?
  int from_server;

  // at step 3, make private pipe
  char pp[HANDSHAKE_BUFFER_SIZE];
  sprintf(pp, "%d", getpid()); // gives pp the PID as a string
  int make = mkfifo(pp, 0644);
  if (make == -1) err();
  printf("client created private pipe %s\n", pp);

  // open wkp[unblock server]
  *to_server = open(WKP, O_WRONLY);
  if (*to_server==-1) err();
  printf("client opened WKP\n");

  // client writing pp to wkp (match w step 5)
  int w = write(*to_server, &pp, HANDSHAKE_BUFFER_SIZE);
  if (w == -1) err();
  printf("client wrote pp to wkp, the client pid\n");

  // client opening PP [BLOCKS]
  from_server = open(pp, O_RDONLY);
  if (from_server == -1) err();
  printf("client opened pp\n");

  // step 8, client deleting pp
  int r = remove(pp);
  if (r==-1) err();
  printf("client removed pp\n");

  // client reading syn_ack (matches w step 7), random number to add 1 to
  int syn_ack;
  int re = read(from_server, &syn_ack, sizeof(syn_ack));
  if (re==-1) err();
  printf("client received syn_ack\n");

  // client sending (matches w step 9) ACK, add 1
  int ack = syn_ack+1;
  int wr = write(*to_server, &ack, sizeof(ack));
  if (wr==-1) err();
  printf("client sent new ack (+1)\n");

  printf("handshake done \n");

  // end
  return from_server;
}

// make half handshake for forking server. all 3 servers should work with the same client (??)

