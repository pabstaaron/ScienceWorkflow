/*
 * Receives UDP packets from the ScienceWorkflow control application
 * and uses them to generate PWM signals to drive the ScienceWorkflow robot.
 *
 * Author: Aaron Pabst
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024
#define PORTNO 8080  // Command listening port

struct sockaddr_in server;
char buf[BUFSIZE]; // Buffer for storing datagrams

void handleCommand(char* cmd);

int main(int argc, char** argv){
  printf("Starting ScienceWorkflow Control Server\n");
  
  int sockfd;

  // Create the listening socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0) // ERROR
    error("ERROR: Could not open socket.");

  // Set port to free up immediatly on progam exit
  int optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));

  // Build server address
  bzero((char *) &server, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons((unsigned short)PORTNO);

  // Bind server address to listening socket
  if (bind(sockfd, (struct sockaddr *) &server, 
	   sizeof(server)) < 0) 
    error("ERROR: Could not bind IP address %x to socket.", server);

  struct sockaddr_in client;
  int clientlen = sizeof(client);
  struct hostent *hostp;
  char *hostaddrp;

  printf("Listening for commands on port %d\n", PORTNO);
  
  // Listen for datagrams forever...
  while(1){
    bzero(buf, BUFSIZE); // Clear datagram buffer
    int n = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr*)&client, &clientlen); // receive datagram
    if(n < 0)
      error("Error: Bad datagram");

    hostp = gethostbyaddr((const char *)&client.sin_addr.s_addr, 
			  sizeof(client.sin_addr.s_addr), AF_INET);
    if (hostp == NULL)
      error("ERROR: Could not retrieve client address");
    hostaddrp = inet_ntoa(client.sin_addr);
    if (hostaddrp == NULL)
      error("ERROR: Could not parse client address");
    
    printf("server received datagram from %s (%s)\n", 
	   hostp->h_name, hostaddrp);
    printf("server received %d/%d bytes: %s\n", strlen(buf), n, buf);

    handleCommand(buf);
  }
}

/*
 * Takes in a command and generates a reactionary control signal
 */
void handleCommand(char* cmd){

}
