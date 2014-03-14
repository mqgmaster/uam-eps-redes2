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
#include "../includes/usersHash.h"
#include "../includes/channelsHash.h"
#include "../includes/command.h"

#define ERROR -1
#define MAXDATASIZE 100 
#define OK 0

int eviarDatos(const void ** msg, int longitud, int socketIdClient);
int recibeDatos(int socketId, void ** msg);
int realizaAccion (int accion, int socketId, char *mensaje);
int cmd_nick(char *mensaje,char**caracter, int socketId);
int cmd_join(char *mensaje,char**caracter, int socketId);
int cmd_privmsg(char *mensaje,char**caracter, int socketId);
int cmd_ping(char *mensaje,char**caracter, int socketId);
int cmd_list(char *msg,char**caracter, int socketId);
int cmd_names(char *msg, char**caracter, int socketId);
int procesarMensaje (char * mensaje, char**caracter, int socketId);