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
//#include "more_string.h"

#define BUFSIZE 1024
#define PORTNO 8080  // Command listening port

struct sockaddr_in server;
char buf[BUFSIZE]; // Buffer for storing datagrams

void handleCommand(char* cmd, struct sockaddr_in client);
void drive(unsigned short theta, unsigned short speed);
void pan(unsigned short theta, unsigned short speed);
void sense_list(struct sockaddr_in client);
void sense_get(char id, struct sockaddr_in client);

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

    handleCommand(buf, client);
  }
}

/*
 * Splits a command string into parameters
 */
char** split_cmd(char* cmd, int* numCmds){
  char** arr = calloc(3, sizeof(char*));

  int i = 0;
  char* pch = strtok(cmd, " ");
  while(pch != NULL){
    if(i > 2)
      return arr;
    
    arr[i] = pch;
    pch = strtok(NULL, " ");
    i++;
  }

  *numCmds = i;
  return arr;
}

// Converts directional arguements encoded in ASCII into their proper numerical formats
unsigned short* parse_dir_cmds(char* theta, char* speed){
  unsigned short stheta = theta[0];
  stheta = stheta << 8;
  stheta = stheta | theta[1];
  
  unsigned short sspeed = speed[0];
  sspeed = sspeed << 8;
  sspeed = sspeed | speed[1];

  unsigned short* arr = malloc(2 * sizeof(unsigned short));
  arr[0] = stheta;
  arr[1] = sspeed;

  return arr;
}

/*
 * Takes in a command and responds accordingly.
 */
void handleCommand(char* cmd, struct sockaddr_in client){
  // Split command about spaces
  int* numargs = malloc(sizeof(int));
  char** scmd = split_cmd(cmd, numargs);

  // remove newline from command, if it is present
  int clen = strlen(scmd[0]);
  if(scmd[0][clen-1] == '\n')
    scmd[0][clen-1] = 0;
  
  if(!strcmp(scmd[0], "DRIVE")){
    if(*numargs != 3)
      return;
    short* args = parse_dir_cmds(scmd[1], scmd[2]);
    drive(args[0], args[1]);
    free(args);
  }
  else if(!strcmp(scmd[0], "PAN")){
    if(*numargs != 3)
      return;
    short* args = parse_dir_cmds(scmd[1], scmd[2]);
    pan(args[0], args[1]);
    free(args);
  }
  else if(!strcmp(scmd[0], "SENSE_LIST")){
    sense_list(client);
  }
  else if(!strcmp(scmd[0], "SENSE_GET")){
    if(*numargs != 2)
      return;
    sense_get(scmd[1][0], client);
  }

  free(scmd);
  free(numargs);
}

void drive(unsigned short theta, unsigned short speed){
  printf("Received drive command with theta = %d and speed = %d\n", theta, speed);
}

void pan(unsigned short theta, unsigned short speed){
  printf("Received pan command with theta = %d and speed = %d\n", theta, speed);
}

void sense_list(struct sockaddr_in client){
  printf("Received sense_list command\n");
}

void sense_get(char id, struct sockaddr_in client){
  printf("Received a sense get command for device %d\n", id);
}
