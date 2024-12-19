#include "pipe_networking.h"
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
  int well_known = mkfifo(WKP, 0644); // er,,,.
  if (well_known==-1) perror("cant make WKP");

  // open wkp.[blocks]
  int open_wkp = open(WKP,  O_WRONLY | O_CREAT | O_TRUNC, 0600); // i dont know about perms
  if (open_wkp==-1) perror("can't open wkp");

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
int server_handshake(int *to_client) {
  int from_client;

  // calls server_setup();
  server_setup();

  // server reading SYN (pid)


  // server opening private pipe [unblock client]
  int open_pp = open("pp",  O_WRONLY | O_CREAT | O_TRUNC, 0600); // i dont know about flags here
  if (open_pp==-1) perror("can't open pp");

  // server send syn_ack

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
int client_handshake(int *to_server) {
  int from_server;

  // at step 3, make private pipe
  int pp = mkfifo("pp", 0644);
  if (pp == -1) perror("private pipe cannot be made");

  int pp_pid;
  if (pp == 0) pp_pid = getpid();

  // open wkp[unblock server]
  int open_wkp = open(WKP,  O_WRONLY | O_CREAT | O_TRUNC, 0600); // i dont know about flags here
  if (open_wkp==-1) perror("can't open wkp");

  // client writing pp to wkp (match w step 5)
  write(open_wkp, pp_pid, 64);

  // step 8, client deleting pp
  remove(pp);

  // client reading syn_ack (matches w step 7)

  // client sending (matches w step 9) ACK

  // end
  return from_server;
}


