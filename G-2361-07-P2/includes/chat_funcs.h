#include "../includes/chat.h"

#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>   

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