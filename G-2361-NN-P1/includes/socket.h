#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define ERROR -1
#define MAXDATASIZE 100 
#define OK 0

struct sockaddr_in infoS, infoC;

int openSocket();
int assignSocket(int socketId,int numPort);
int openQueue(int socketId, int longQueue);
int closeServer(int socketId);	
int cerrarSesion(int socketId);
int startListening(int socketIdc);