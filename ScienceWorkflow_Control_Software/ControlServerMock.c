/*
 * Stripped down version of the SciWorkflow robot control server for testing purposes.
 * Has no PWM control capabilities, and is only capable of receiving and sending commands based on the ControlAPI.
 *
 * Author: Aaron Pabst
 */

//#include <python2.7/Python.h>
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
#include <time.h>

#define BUFSIZE 1024
#define PORTNO 9090  // Command listening port

// Sensor type associations
#define USHORT 0

int sockfd;
struct sockaddr_in server;
char buf[BUFSIZE]; // Buffer for storing datagrams

void handleCommand(char* cmd, struct sockaddr_in client);
void drive(unsigned short theta, unsigned short speed);
void pan(unsigned short theta, unsigned short speed);
void sense_list(struct sockaddr_in client);
void sense_get(char id, struct sockaddr_in client);
void build_dummy_sensors();
int send_udp(struct sockaddr_in client, char* buf2);
char* trimStr(char* str);
/*void call_set_pwm_freq(int freq);
void call_set_pwm(int channel, int on, int off);
void call_set_all_pwm(int on, int off);*/

// Represents a sensor and it's associated fields
typedef struct{
  char id;
  char type;
  char* name;
} sensor;

sensor* sensors; // Sensor array

// Python execution variables
/*const char* adafruit_pwm_lib = "PCA9685"; // Need to make sure this is on python path
PyObject* soft_reset; // Resets the PWM board, no args
PyObject* set_pwm_freq; // Sets the PWM frequency, freq_hz
PyObject* set_pwm; // Sets a single PWM channel, channel, on, off
PyObject* set_all_pwm; // Sets all PWM channels, on, off*/

const char* soft_reset_name = "software_reset";
const char* set_pwm_freq_name = "set_pwm_freq";
const char* set_pwm_name = "set_pwm";
const char* set_all_pwm_name = "set_all_pwm";

void error(char* msg){
  perror(msg);
  exit(1);
}

int main(int argc, char** argv){
  printf("Starting ScienceWorkflow Control Server\n");
  
  // Build sensor dictionary
  sensors = calloc(256, sizeof(sensor)); // Set up a sensor data structure with a direct id mapping
  build_dummy_sensors();
  
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
	   sizeof(server)) < 0){
    error("ERROR: Could not bind IP address to socket.");
  }

  struct sockaddr_in client;
  int clientlen = sizeof(client);
  struct hostent *hostp;
  char* hostaddrp;

  printf("Listening for commands on port %d\n", PORTNO);
  
  // Listen for datagrams forever...
  while(1){
    bzero(buf, BUFSIZE); // Flush datagram buffer
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

  // Py_Finalize();
}

char* grow(char *str){
  char* new = malloc(strlen(str) + 1);
  for(int i = 0; i < strlen(str); i++)
    new[i] = str[i];

  return new;
}

/**
 * Trims excess null-terminators from a string
 */
char* trimStr(char* str){
  char* new = malloc(1); // Starting array
  int i = 0;
  while(1){
    new[i] = str[i];
    if(str[i] == 0)
      break;
    new = grow(new);
    i++;
  }

  return new;
}

void build_dummy_sensors(){
  sensor* accel = (sensor*)malloc(sizeof(sensor));
  accel->id = 0;
  accel->name = "accelorometer";
  accel->type = USHORT;
  sensors[0] = *accel;

  sensor* gyro = (sensor*)malloc(sizeof(sensor));
  gyro->id = 1;
  gyro->name = "gyroscope";
  gyro->type = USHORT;
  sensors[1] = *gyro;
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

// Converts directional arguments encoded in ASCII into their proper numerical formats
unsigned short* parse_dir_cmds(char* theta, char* speed){
  short stheata = atoi(theta);
  short sspeed = atoi(speed);
  
  unsigned short* arr = malloc(2 * sizeof(unsigned short));

  arr[0] = stheata;
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
  
  if(!strcmp(scmd[0], "DRIVE")){ // If we've received a drive command
    if(*numargs != 3){ // Ensure instruction is of the expected format
      printf("Excpected two arguments for DRIVE command, got %d\n", *numargs-1);
      return;
    }
    short* args = parse_dir_cmds(scmd[1], scmd[2]);
    drive(args[0], args[1]);
    free(args);
  }
  else if(!strcmp(scmd[0], "PAN")){ // If we've received a PAN command
    if(*numargs != 3){
      printf("Excpected two arguments for PAN command, got %d\n", *numargs-1);
      return;
    }
    short* args = parse_dir_cmds(scmd[1], scmd[2]);
    pan(args[0], args[1]);
    free(args);
  }
  else if(!strcmp(scmd[0], "SENSE_LIST")){ // If we've received a SENSE_LIST command
    //if(*numargs != 1)
      sense_list(client);
  }
  else if(!strcmp(scmd[0], "SENSE_GET")){ // If we've received a SENSE_GET command
    if(*numargs != 2){
      printf("Excpected one arguments for SENSE_GET command, got %d\n", *numargs-1);
      return;
    }
    sense_get(atoi(scmd[1]), client);
  }

  // Cleanup
  free(scmd);
  free(numargs);
}

/*
 * Alter PWM parameters to match the specified speed and direction parameters
 */
void drive(unsigned short theta, unsigned short speed){
  printf("Received drive command with theta = %d and speed = %d\n", theta, speed);
}

/*
 * Alter PWM parameters to begin adjust the camera 
 */
void pan(unsigned short theta, unsigned short speed){
  printf("Received pan command with theta = %d and speed = %d\n", theta, speed);
}

char* growString(char* str, char* toAdd, int* avail){
  int nlen = strlen(toAdd);
  
  while(nlen >= *avail){
    str = realloc(str, *avail + 100);
    *avail += 100;
  }

  return str;
}

char* build_sensor_list(){
  int avail = 100;
  char* buf = calloc(avail, sizeof(char));
  
  for(int i = 0; i < 256; i++){
    if(sensors[i].name != NULL){
      
      growString(buf, sensors[i].name, &avail);
      buf = strcat(buf, sensors[i].name);
      buf = strcat(buf, " ");
      avail -= strlen(sensors[i].name) + 1;
      
      char* id = calloc(2, sizeof(char));
      sprintf(id, "%d", sensors[i].id);
      growString(buf, id, &avail) ;
      buf = strcat(buf, id);
      buf = strcat(buf, " ");
      avail -= strlen(id) + 1;
      
      char* toAdd;
      switch(sensors[i].type){
      case USHORT:
	toAdd = "ushort";
	growString(buf, toAdd, &avail); 
	buf = strcat(buf, toAdd);
	avail -= strlen(toAdd);
	break;
      default:
	toAdd = "whoops";
	growString(buf, toAdd, &avail);
	buf = strcat(buf, toAdd);
	avail -= strlen(toAdd);
	break;
      }

      toAdd = "\n";
      growString(buf, toAdd, &avail);
      buf = strcat(buf, toAdd);
      avail -= 1;

      free(id);
    }
  }

  return buf;
}

/*
 * Send a list of all available sensors to client as plain text
 */
void sense_list(struct sockaddr_in client){
  printf("Received sense_list command\n");

  char* list = build_sensor_list();
  //printf("%s\n", list);

  int bytesSent = send_udp(client, list);
  printf("Bytes sent: %d\n", bytesSent);
  
  free(list);
}

/*
 * Send an individual sensor sample back to client
 */
void sense_get(char id, struct sockaddr_in client){
  printf("Received a sense get command for device %d\n", id);

  if(sensors[id].name != NULL){
    // Just send some garbage data for now
    //char* data = malloc(2);
    srand(time(0));
    short num = rand();

    char* data = malloc(15);
    sprintf(data, "%d", num);

    data = trimStr(data);

    printf("Sent sample %s\n", data);
    
    send_udp(client, data);
    free(data);
  }
}

/*
 * Sends a udp packet with buf as a payload
 */
int send_udp(struct sockaddr_in client, char* buf2){
  int len = sizeof(client);
  return sendto(sockfd, buf2, strlen(buf2), 0, (struct sockaddr*) &client, len);
}

