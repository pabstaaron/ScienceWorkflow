/*
 * Receives UDP packets from the ScienceWorkflow control application
 * and uses them to generate PWM signals to drive the ScienceWorkflow robot.
 *
 * Author: Aaron Pabst
 */

#include <python2.7/Python.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>

#define BUFSIZE 1024
#define PORTNO 8080  // Command listening port

// Sensor type associations
#define USHORT 0

#define PWM_FREQ 60 
#define SERVO_MIN 150 // Fastest Clockwise direction
#define SERVO_MAX 600 // Fastest counter-clockwise direction

// Directional servo extremes
#define DIR_SERVO_MIN 0
#define DIR_SERVO_MAX 360

#define LIN_SERVO_MAX 600 // 2 ms mark
#define LIN_SERVO_MIN 150 // 1 ms mark

#define MAX_SPEED 4095

// Wheel to channel mappings
#define FRNT_LEFT 0
#define FRNT_RIGHT 1
#define BCK_LEFT 2 
#define BCK_RIGHT 3

// Pan/tilt mappings
#define PT_HOR 4 // Horizontal adjustment servo
#define PT_VERT 6 // Vertical adjustment servo

#define MOTOR_DELAY 125000000 // The number of nanoseconds to wait between motor adjustments

int sockfd;
struct sockaddr_in server;
char buf[BUFSIZE]; // Buffer for storing datagrams

void handleCommand(char* cmd, struct sockaddr_in client);
void drive(unsigned short theta, unsigned short speed);
void pan(unsigned short x, unsigned short y);
void sense_list(struct sockaddr_in client);
void sense_get(char id, struct sockaddr_in client);
void build_dummy_sensors();
int send_udp(struct sockaddr_in client, char* buf2);
void call_set_pwm_freq(int freq);
void call_set_pwm(int channel, int on, int off);
void call_set_all_pwm(int on, int off);
long map(long val, long fromLow, long fromHigh,long toLow, long toHigh);
void servoWrite(int channel, int value);
void linearWrite(int channel, int value);
void delayNano(int nanoseconds);

// Represents a sensor and it's associated fields
typedef struct{
  char id;
  char type;
  char* name;
} sensor;

sensor* sensors; // Sensor array

// Python execution variables
const char* adafruit_pwm_lib = "Adafruit_PCA9685"; // Need to make sure this is on python path
const char* pwm_obj_name = "PCA9685";
//PyObject* soft_reset; // Resets the PWM board, no args
//PyObject* set_pwm_freq; // Sets the PWM frequency, freq_hz
//PyObject* set_pwm; // Sets a single PWM channel, channel, on, off
//PyObject* set_all_pwm; // Sets all PWM channels, on, off

const char* soft_reset_name = "software_reset";
const char* set_pwm_freq_name = "set_pwm_freq";
const char* set_pwm_name = "set_pwm";
const char* set_all_pwm_name = "set_all_pwm";

PyObject* instance;

void error(char* msg){
  perror(msg);
  exit(1);
}

static void segHandler(int sig, siginfo_t *si, void* ptr){
  if(PyErr_Occurred())
    PyErr_PrintEx(1);
  error("Segmentation Fault");
}

int main(int argc, char** argv){
  printf("Starting ScienceWorkflow Control Server\n");
  
  // Start up the python interpreter 
  Py_Initialize();

  // Set up error handling
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = segHandler;
  if (sigaction(SIGSEGV, &sa, NULL) == -1)
    error("Couldn't install error handler");
  

  // Register PWM library
  PyObject* p_name, *pModule;
  
  p_name = PyString_FromString(adafruit_pwm_lib);
  pModule = PyImport_Import(p_name);
  Py_DECREF(p_name); // We're done with p_name, so release it.

  if(pModule == NULL){
    error("Module undefined! Is the PCA9685 library on the Python path?");
  }

  /* PyObject* init = PyObject_GetAttrString(pModule, "__init__"); */

  /* if(init == NULL) */
  /*   error("Couldn't find python defintition for function init."); */

  // Make an instance of the PCA9685 module
  PyObject* PCA9685 = PyObject_GetAttrString(pModule, pwm_obj_name);
  if(!PyCallable_Check(PCA9685)){
    if(PyErr_Occurred())
      PyErr_PrintEx(1);
    error("Can't call PCA9685");
  }
  
  
  instance = PyObject_CallObject(PCA9685, NULL);
    if(instance == NULL){
      if(PyErr_Occurred())
	PyErr_PrintEx(1);
    error("Could not instantiate PCA9685 module!");
    }

  
  // Set up Python function refs
  //soft_reset = PyObject_GetAttrString(instance, soft_reset_name);
  //set_pwm_freq = PyObject_GetAttrString(instance, set_pwm_freq_name);
  //set_pwm = PyObject_GetAttrString(instance, set_pwm_name);
  //set_all_pwm = PyObject_GetAttrString(instance, set_all_pwm_name);

  if(PyErr_Occurred()){
    PyErr_PrintEx(1);
    error("Python error occured.");
  }

  // Initilize all PWM channels
  call_set_pwm_freq(PWM_FREQ);

  pan(90, 90); // set camera straight forward
  drive(0, 0); // Set drive to full stop
  
  // Build sensor dictionary
  sensors = calloc(256, sizeof(sensor)); // Set up a sensor data structure with a direct id mapping. // TODO: Replace this with a less memory intensive solution
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
    if(i > 3)
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
    if(*numargs != 3) // Ensure instruction is of the expected format
      return;
    short* args = parse_dir_cmds(scmd[1], scmd[2]);
    drive(args[0], args[1]);
    free(args);
  }
  else if(!strcmp(scmd[0], "PAN")){ // If we've received a PAN command
    if(*numargs != 3)
      return;
    short* args = parse_dir_cmds(scmd[1], scmd[2]);
    pan(args[0], args[1]);
    free(args);
  }
  else if(!strcmp(scmd[0], "SENSE_LIST")){ // If we've received a SENSE_LIST command
    //if(*numargs != 1)
      sense_list(client);
  }
  else if(!strcmp(scmd[0], "SENSE_GET")){ // If we've received a SENSE_GET command
    if(*numargs != 2)
      return;
    sense_get(atoi(scmd[1]), client);
  }
  else if(!strcmp(scmd[0], "PWM")){ // PWM [CH] [START] [STOP]
    //if(*numargs != 4)
    //return;
      printf("Received PWM Command!\n");
      int ch = atoi(scmd[1]);
      short start = atoi(scmd[2]);
      short end = atoi(scmd[3]);
      call_set_pwm(ch, start, end);
  }
  else if(!strcmp(scmd[0], "WRITE")){
    int ch = atoi(scmd[1]);
    int pos = atoi(scmd[2]);
    servoWrite(ch, pos);
  }

  // Cleanup
  free(scmd);
  free(numargs);
}

/*
 * Alter PWM parameters parameters to match the specified speed and direction parameters
 */
void drive(unsigned short theta, unsigned short speed){
  printf("Received drive command with theta = %d and speed = %d\n", theta, speed);
  //call_set_pwm(FRNT_LEFT, 0, speed);
 
  int newSpeed; 

  if(speed == 0){
    servoWrite(FRNT_LEFT, 180);
    servoWrite(FRNT_RIGHT, 180);
    servoWrite(BCK_LEFT, 180);
    servoWrite(BCK_RIGHT, 180);
    return;
  }

  if(theta == 0){
    int s = map(speed, 0, 4096, 180, 0);
    servoWrite(FRNT_LEFT, s);
    servoWrite(FRNT_RIGHT, s);
    servoWrite(BCK_LEFT, s);
    servoWrite(BCK_RIGHT, s);
    return;
  }

  if(theta == 180){
    int s = map(speed, 0, 4096,180, 360);
    servoWrite(FRNT_LEFT, s);
    delayNano(MOTOR_DELAY);
    servoWrite(FRNT_RIGHT, s);
    delayNano(MOTOR_DELAY);
    servoWrite(BCK_LEFT, s);
    delayNano(MOTOR_DELAY);
    servoWrite(BCK_RIGHT, s);
    return;
  }

  if((theta >= 0 && theta <= 90) || (theta >= 270 && theta <= 360)){ // Forward
    newSpeed = map(speed, 0, 4096, 0, 180);
    if(theta >= 0 && theta <= 90){ // Left
      float scale = (float)theta / 90.0;
      int leftSpeed = (int)(scale * speed);
      int rightSpeed = speed;
      leftSpeed = map(leftSpeed, 0, 4096, 180, 0);
      rightSpeed = map(rightSpeed, 0, 4096, 180, 0);
      servoWrite(FRNT_LEFT, leftSpeed);
      servoWrite(FRNT_RIGHT, rightSpeed);
      servoWrite(BCK_LEFT, leftSpeed);
      servoWrite(BCK_RIGHT, rightSpeed);
    }
    else{ // Right
      float scale = (float)(theta - 270) / 90.0;
      int rightSpeed = (int)(scale * speed);
      int leftSpeed = speed;
      rightSpeed = map(rightSpeed, 0, 4096, 180, 0);
      leftSpeed = map(leftSpeed, 0, 4096, 180, 0);
      servoWrite(FRNT_LEFT, leftSpeed);
      servoWrite(FRNT_RIGHT, rightSpeed);
      servoWrite(BCK_LEFT, leftSpeed);
      servoWrite(BCK_RIGHT, rightSpeed);
    }
  }
  else{ // backwards
    //newSpeed = map(speed, 0, 4096, 180, 360);
    if(theta > 90 || theta <= 180){ // Left
      float scale = (float)(theta-90) / 90.0;
      int leftSpeed = (int)(scale * speed);
      int rightSpeed = speed;
      leftSpeed = map(leftSpeed, 0, 4096, 180, 360);
      rightSpeed = map(rightSpeed, 0, 4096, 180, 360);
      servoWrite(FRNT_LEFT, leftSpeed);
      servoWrite(FRNT_RIGHT, rightSpeed);
      servoWrite(BCK_LEFT, leftSpeed);
      servoWrite(BCK_RIGHT, rightSpeed);
    }
    else{ // Right
      float scale = (float)(theta-180) / 90.0;
      int rightSpeed = (int)(scale * speed);
      int leftSpeed = speed;
      leftSpeed = map(leftSpeed, 0, 4096, 180, 360);
      rightSpeed = map(rightSpeed, 0, 4096, 180, 360);
      servoWrite(FRNT_LEFT, leftSpeed);
      servoWrite(FRNT_RIGHT, rightSpeed);
      servoWrite(BCK_LEFT, leftSpeed);
      servoWrite(BCK_RIGHT, rightSpeed);
    }
  }
}

/*
 * Alter PWM parameters to begin adjust the camera 
 * x and y should be an integer between 0 and 180
 */
void pan(unsigned short x, unsigned short y){
  printf("Received pan command with x = %d and y =  %d\n", x, y);
  linearWrite(PT_HOR, x);
  linearWrite(PT_VERT, y);
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
  
  int i;
  for(i = 0; i < 256; i++){
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
  printf("%s\n", list);

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
    char* data = malloc(2);
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

// Adafruit PWM Wrappers
// https://learn.adafruit.com/adafruit-16-channel-servo-driver-with-raspberry-pi/library-reference

/*
 * Sets the PWM Frequency.
 * freq: an integer value, in Hz, from 40 to 1000 
 */
void call_set_pwm_freq(int freq){
  //Py_Initialize();

  // Register PWM library
  /*PyObject* p_name, *pModule;

  p_name = PyString_FromString(adafruit_pwm_lib);
  pModule = PyImport_Import(p_name);

  if(pModule == NULL){
    error("Module undefined! Is the PCA9685 library on the Python path?");
  }
  
  // Make an instance of the PCA9685 module
  PyObject* PCA9685 = PyObject_GetAttrString(pModule, pwm_obj_name);
  if(!PyCallable_Check(PCA9685)){
    if(PyErr_Occurred())
      PyErr_PrintEx(1);
    error("Can't call PCA9685");
  } 
                                                                                                        
   PyObject* instance = PyObject_CallObject(PCA9685, NULL);                                                
   if(instance == NULL){                                                                                   
     if(PyErr_Occurred())
       PyErr_PrintEx(1);                                                                                  
     error("Could not instantiate PCA9685 module!");                                                      
     }                                                                                    */                  

  PyObject* set_pwm_freq = PyObject_GetAttrString(instance, set_pwm_freq_name);
  
  PyObject *pargs = PyTuple_New(1);
  PyObject *pint = PyInt_FromLong(freq);
  PyTuple_SetItem(pargs, 0, pint);

  PyObject_CallObject(set_pwm_freq, pargs);

  // Cleanup
  //Py_DECREF(p_name);
  //Py_DECREF(pModule);
  //Py_DECREF(PCA9685);
  //Py_DECREF(instance);
  Py_DECREF(set_pwm_freq);
  Py_DECREF(pargs);
  //Py_DECREF(pint);
 
  //Py_Finalize();
}

/*
 * Sets the on/off interval on channel.
 * channel: The channel to be updated. An integer between 0 and 15
 * on: The count value to activate the pulse. An integer between 0 and 4095.
 * off: The count value to deactivate the pulse. An integer between 0 and 4095
 */
void call_set_pwm(int channel, int on, int off){
  
  //Py_Initialize();

  // Register PWM library
  /*PyObject* p_name, *pModule;

  p_name = PyString_FromString(adafruit_pwm_lib);
  pModule = PyImport_Import(p_name);

  if(pModule == NULL){
    error("Module undefined! Is the PCA9685 library on the Python path?");
  }
  
  
  // Make an instance of the PCA9685 module
  PyObject* PCA9685 = PyObject_GetAttrString(pModule, pwm_obj_name);
  if(!PyCallable_Check(PCA9685)){
    if(PyErr_Occurred())
      PyErr_PrintEx(1);
    error("Can't call PCA9685");
  }

  PyObject* instance = PyObject_CallObject(PCA9685, NULL);
  if(instance == NULL){
    if(PyErr_Occurred())
      PyErr_PrintEx(1);
    error("Could not instantiate PCA9685 module!");
    }*/
  
  PyObject* set_pwm = PyObject_GetAttrString(instance, set_pwm_name);
  
  PyObject *pargs = PyTuple_New(3);

  PyObject *pchannel = PyInt_FromLong(channel);
  PyObject *pon = PyInt_FromLong(on);
  PyObject *poff = PyInt_FromLong(off);

  PyTuple_SetItem(pargs, 0, pchannel);
  PyTuple_SetItem(pargs, 1, pon);
  PyTuple_SetItem(pargs, 2, poff);

  PyObject_CallObject(set_pwm, pargs);

  //Py_DECREF(p_name);
  //Py_DECREF(pModule);
  //Py_DECREF(PCA9685);
  //Py_DECREF(instance);
  Py_DECREF(set_pwm);
  Py_DECREF(pargs);
  //Py_DECREF(pchannel);
  //Py_DECREF(pon);
  //Py_DECREF(poff);

  //Py_Finalize();
}

void call_set_all_pwm(int on, int off){
  //Py_Initialize();

  // Register PWM library
  /*PyObject* p_name, *pModule;

  p_name = PyString_FromString(adafruit_pwm_lib);
  pModule = PyImport_Import(p_name);

  if(pModule == NULL){
    error("Module undefined! Is the PCA9685 library on the Python path?");
  }
  
  // Make an instance of the PCA9685 module
  PyObject* PCA9685 = PyObject_GetAttrString(pModule, pwm_obj_name);
  if(!PyCallable_Check(PCA9685)){
    if(PyErr_Occurred())
      PyErr_PrintEx(1);
    error("Can't call PCA9685");
  }

  PyObject* instance = PyObject_CallObject(PCA9685, NULL);
  if(instance == NULL){
    if(PyErr_Occurred())
      PyErr_PrintEx(1);
    error("Could not instantiate PCA9685 module!");
    }*/

  PyObject* set_all_pwm = PyObject_GetAttrString(instance, set_all_pwm_name);
  
  PyObject *pargs = PyTuple_New(2);

  PyObject *pon = PyInt_FromLong(on);
  PyObject *poff = PyInt_FromLong(off);

  PyTuple_SetItem(pargs, 0, pon);
  PyTuple_SetItem(pargs, 1, poff);

  PyObject_CallObject(set_all_pwm, pargs);

  //Py_DECREF(p_name);
  //Py_DECREF(pModule);
  //Py_DECREF(PCA9685);
  //Py_DECREF(instance);
  Py_DECREF(set_all_pwm);
  Py_DECREF(pargs);
  //Py_DECREF(pon);
  //Py_DECREF(poff);

  //Py_Finalize();
}

/*
 * Maps a value from one number range onto another
 */
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/*
 * Similar to Arduino's Servo.write()
 * Takes in a positional arguement between DIR_SERVO_MIN and DIR_SERVO_MAX degrees.
 * Where DIR_SERVO_MIN is the rightmost servo position or the maximum angular velocity.
 * DIR_SERVO_MAX is the leftmost position or the minimum angular velocity.
 */
void servoWrite(int channel, int value){
  if(value < DIR_SERVO_MIN)
    value = DIR_SERVO_MIN;
  if(value > DIR_SERVO_MAX)
    value = DIR_SERVO_MAX;

  int pwm = map(value, DIR_SERVO_MIN, DIR_SERVO_MAX, SERVO_MIN, SERVO_MAX);

  call_set_pwm(channel, 0, pwm);

  //delayNano(999999999);
}

void linearWrite(int channel, int value){
  if(value < 0)
    value = 0;
  if(value > 180)
    value = 180;

  int pwm = map(value, 0, 180, LIN_SERVO_MIN, LIN_SERVO_MAX);

  call_set_pwm(channel, 0, pwm);

  //delayNano(MOTOR_DELAY);
}

/*
 * Locks up the main thread for a given number of nanoseconds
 */
void delayNano(int nanoseconds){
  struct timespec* reqTime = malloc(sizeof(struct timespec)); // Allocate a timespec object
  reqTime->tv_sec = 0;
  reqTime->tv_nsec = nanoseconds;

  struct timespec* remTime = malloc(sizeof(struct timespec)); // dummy pointer

  nanosleep(reqTime, remTime);

  free(reqTime);
  free(remTime);
}
