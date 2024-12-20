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
  from_client = mkfifo(WKP, 0644); // er,,,.
  if (from_client==-1) err();

  // open wkp.[blocks]
  int open_wkp = open(from_client,  O_RDONLY, 0644); // i dont know about perms
  if (open_wkp==-1) err();

  // step 3 in client

  // remove the wkp
  remove(WKP);

  return from_client;
}

/*=========================
  server_handshake 
  args: int * to_client

  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe (Client's private pipe).

  returns the file descriptor for the upstream pipe (see server setup).
  =========================*/
int server_handshake(int *to_client) { // im confused... do i use to_server or to_client for wkp and pp? :P to_client = pp...?
  int from_client;

  // calls server_setup();
  server_setup();

  // server reading SYN (pid)
  int my_syn = getpid();

  // server opening private pipe [unblock client]
  from_client = open("pp",  O_WRONLY | O_CREAT, 0600); // i dont know about flags here
  if (from_client==-1) err();

  // server send syn_ack
  int w = write(from_client, my_syn, 64);
  if (w==-1) err();

  // step 8 in client

  // server received ack, handshake complete
  // end server_handshake()

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
  int pp = mkfifo("pp", 0644);
  if (pp == -1) err();

  int pp_pid;
  if (pp == 0) pp_pid = getpid();

  // open wkp[unblock server]
  int open_wkp = open(WKP,  O_WRONLY | O_CREAT | O_TRUNC, 0600); // i dont know about flags here
  if (open_wkp==-1) err();

  // client writing pp to wkp (match w step 5)
  int w = write(open_wkp, pp_pid, 64);
  if (w == -1) err();

  // step 8, client deleting pp
  int r = remove(pp);
  if (r==-1) err();

  // client reading syn_ack (matches w step 7)

  // client sending (matches w step 9) ACK

  // end
  return from_server;
}


