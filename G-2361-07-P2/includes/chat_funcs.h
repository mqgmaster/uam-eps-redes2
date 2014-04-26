#include "../includes/chat.h"

#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <netdb.h> 
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <errno.h>
#include <fcntl.h>

int fd;
struct hostent *he;
struct sockaddr_in server;

void procesaMensaje(char* mensaje, char* contestacion);
void connectClient(void);
void disconnectClient(void);
void topicProtect(gboolean state);
void externMsg(gboolean state);
void secret(gboolean state);
void guests(gboolean state);
void privated(gboolean state);
void moderated(gboolean state);
void newText (const char *msg);

void sendData(const char *string);
void receiveData(char *msg);
void * audioReceiver();
void * audioReceiver1();
void startListeningThreadAudio1();
void startListeningThreadAudio() ;
void send_msg(char* data);
void send_sound(char* data);
void send_join(char* data);
void send_nick(char* data);
void send_part(char* data);
void send_quit();
void send_kick(char * data);
void send_mode(char * data);
void send_ban(char * data);
void receive_msg(char* data);
void receive_join(char* data);
void receive_part(char* data);


void sendAudio(const char *string);
void recibeAudio(char *msg);

void startListeningThread();
void* messageReceiver();

void removeChar(char *str, char garbage);
