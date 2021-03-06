#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define	MY_PORT	2222

/* ---------------------------------------------------------------------
 This	is  a sample server which opens a stream socket and then awaits
 requests coming from client processes. In response for a request, the
 server sends an integer number  such	 that  different  processes  get
 distinct numbers. The server and the clients may run on different ma-
 chines.
 --------------------------------------------------------------------- */

int main()
{
  int sock, snew, fromlength, outnum, i;
  char tempmessage[1024], message[2048];
  struct sockaddr_in master, from;
  

  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror ("Server: cannot open master socket");
    exit (1);
  }
  
  master.sin_family = AF_INET;
  master.sin_addr.s_addr = INADDR_ANY;
  master.sin_port = htons (MY_PORT);
  
  if (bind (sock, (struct sockaddr*) &master, sizeof (master))) {
    perror ("Server: cannot bind master socket");
    exit (1);
  }
  
  listen (sock, 5);
  
  while(1) {
    fromlength = sizeof (from);
    snew = accept (sock, (struct sockaddr*) & from, & fromlength);
    if (snew < 0) {
      perror ("Server: accept failed");
      exit (1);
    }
    
    strcpy(tempmessage, "CMPUT379 Whiteboard Server v0\n");
    send(snew, tempmessage, sizeof(tempmessage), 0);
    
  
  //outnum = htonl (number);
  //write (snew, &outnum, sizeof (outnum));
  
  int n = recv(snew, message, sizeof(message), 0);
  printf("message = %s\n", message);
  
  send(snew, message, sizeof(message), 0);
  //  send(sock, message, sizeof(message), 0);

  
  close (snew);
  //number++;
  
  }

}


// int n = recv(sock, c, sizeof(c), 0) n is the size of the message
