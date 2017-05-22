#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "dictionary.h"

#define BUFSIZE 1024

int sockfd;

int main(){
  int sockfd;
  int portno = 8080;
  int servlen;

  char *hostname = "0.0.0.0";
  char buf[1024];
  
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0) 
    error("ERROR opening socket");
  
  struct sockaddr_in serveraddr;
  struct hostent *server;

  // Look up server through DNS
  server = gethostbyname(hostname);
  if (server == NULL) {
      fprintf(stderr,"ERROR, no such host as %s\n", hostname);
      exit(0);
  }

  // build Server address
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, 
	(char *)&serveraddr.sin_addr.s_addr, server->h_length);
  serveraddr.sin_port = htons(portno);

  char* cmd = "PAN 10 1000";
  

  printf("Sending message to Server\n");
  // Send msg to server
  int serverlen = sizeof(serveraddr);
  int send = sendto(sockfd, cmd, strlen(cmd), 0, (struct sockaddr*)&serveraddr, serverlen);
  printf("sent: %d\n", send);

  // Receive response
  printf("strlen(buf)=%d\n", strlen(buf));
  int recv = recvfrom(sockfd, buf, 1024, 0, (struct sockaddr*)&serveraddr, &serverlen);
  printf("Bytes received: %d\n", recv);

  printf("%s\n", buf);
}
