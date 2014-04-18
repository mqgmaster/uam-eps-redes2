#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>   
#include "chat.h"

int fd;
struct hostent *he;
struct sockaddr_in server;

void procesaMensaje(char* mensaje, char* contestacion){
	char *ptr,*mensaje1, *mensaje2, auxiliar[500];
	mensaje1 = strtok(mensaje, ":");
	while((mensaje2 = strtok(NULL, ":"))!=NULL){
		if(strstr(mensaje1,"PING")!=NULL){
			sprintf(contestacion, "PONG : %s\r\n",mensaje2);
		}else{
			sprintf(contestacion, "%s ",mensaje2);
			
		}
	}
	printf("contestacion: %s\n",contestacion);
}

void connectClient(void)
{

	int puerto=-1;
	char *ip, *user, *nick, *nombre,*contestacion,*ptr,*ptr1;
	int numbytes, longitud, enviados,aux =0;       
	char buf[3000], mensaje[512], cadena[100];
	  
	ip = (char*) calloc(100,sizeof(char));
	user = (char*) calloc(25,sizeof(char));
	nick = (char*) calloc(25,sizeof(char));
	nombre = (char*) calloc(25,sizeof(char));
	contestacion =(char*) calloc(512,sizeof(char));
	if(ip==NULL || user == NULL || nick == NULL || nombre == NULL){
		printf("Error en ip, user o nick\n");
		exit(-1);
	}

	ip = getServidor();
	puerto=getPuerto();
	if(puerto == -1){
		printf("Error en puerto\n");
		exit(-1);
	}	
		
	nick = getApodo();
	user = getNombre();
	nombre = getNombreReal();

	he = gethostbyname(ip);
	if(he == NULL){
		errorText("Error al coger el host\n");
		exit(-1);
	}

	if ((fd=socket(AF_INET, SOCK_STREAM, 0))==-1){  
		printf("Error al abrir el socket\n");		
		exit(-1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(puerto);
	server.sin_addr =*((struct in_addr *) he->h_addr);
	bzero(&(server.sin_zero),8);
	
	if(connect(fd,(struct sockaddr *)&server, sizeof(struct sockaddr))==-1){
		printf("Error al conectar con el servidor\n");		
		exit(-1);
	}
	if ((numbytes=recv(fd,buf,512,0)) == -1){  
		exit(-1);
	}

	buf[numbytes]='\0';

	procesaMensaje(buf,contestacion);
	
	messageText(contestacion);

	sprintf(mensaje, "NICK %s\r\nUSER %s %s %s :%s\r\n",nick,user,user, ip, nombre);

	longitud = strlen(mensaje);
	
	enviados = send(fd, mensaje, longitud, 0);
	if (enviados <= 0){
	    printf("Error al enviar datos\n");
	    exit(-1);
	}

	if ((numbytes=recv(fd,buf,512,0)) == -1){  
		exit(-1);
	}
	buf[numbytes]='\0';
	
	procesaMensaje(buf,contestacion);
	enviados = send(fd, contestacion, strlen(contestacion), 0);
	if (enviados <= 0){
	    printf("Error al enviar datos\n");
	    exit(-1);
	}

	
	if((numbytes=recv(fd,buf,1500,0)) == -1){
		printf("Error al recibir 2\n");
		exit(-1);
	}

	ptr= strtok(buf,"\r\n");
	strcpy(cadena,ptr);
	messageText(cadena);
	while((ptr1 = strtok(NULL, "\r\n"))!= NULL){
		printf("%s\n",ptr1);
		strcpy(cadena,ptr1);
		messageText(cadena);
	}

	if((numbytes=recv(fd,buf,1500,0)) == -1){
		printf("Error al recibir 2\n");
		exit(-1);
	}

	ptr= strtok(buf,"\r\n");
	strcpy(cadena,ptr);
	messageText(cadena);
	while((ptr1 = strtok(NULL, "\r\n"))!= NULL){
		printf("%s\n",ptr1);
		strcpy(cadena,ptr1);
		messageText(cadena);
	}
	
}

void disconnectClient(void)
{
	char mensaje[512],buf[512];
	int enviados,longitud,numbytes;
	sprintf(mensaje, "QUIT : Saliendo\r\n");

	longitud = strlen(mensaje);
	
	enviados = send(fd, mensaje, longitud, 0);
	if (enviados <= 0){
	    printf("Error al enviar datos\n");
	    exit(-1);
	}
	if((numbytes=recv(fd,buf,1500,0)) == -1){
		printf("Error al recibir 2\n");
		exit(-1);
	}
	messageText(buf);
	close(fd);
}

void topicProtect(gboolean state)
{
	setTopicProtect(state);
}

void externMsg(gboolean state)
{
	setExternMsg(state);
}

void secret(gboolean state)
{
	setSecret(state);
}

void guests(gboolean state)
{
	setGuests(state);
}

void privated(gboolean state)
{
	setPrivate(state);
}

void moderated(gboolean state)
{
	setModerated(state);
}

void newText (const char *msg)
{
	int longitud, enviados,numbytes;
	char *ptr,*mensaje,buf[512],*canal;
	mensaje = (char*) calloc (512, sizeof(char));

	ptr = strtok(msg,"/");
	if(strstr (ptr,"join")!= NULL){
		canal = strtok(ptr," "); //join
		canal = strtok(NULL,"\r\n"); //canal
		//join
		sprintf(mensaje,"JOIN %s\r\n",canal);
		longitud = strlen(mensaje);
		enviados = send(fd, mensaje, longitud, 0);
		if (enviados <= 0){
		    printf("Error al enviar datos\n");
		    exit(-1);
		}
		if((numbytes=recv(fd,buf,512,0)) == -1){
			printf("Error al recibir 2\n");
			exit(-1);
		}
messageText(buf);
		//mode
		sprintf(mensaje,"MODE %s\r\n",canal);	
		longitud = strlen(mensaje);
		enviados = send(fd, mensaje, longitud, 0);
		if (enviados <= 0){
		    printf("Error al enviar datos\n");
		    exit(-1);
		}
		if((numbytes=recv(fd,buf,512,0)) == -1){
			printf("Error al recibir 2\n");
			exit(-1);
		}
messageText(buf);
		//who
		sprintf(mensaje,"WHO %s\r\n",canal);	
		longitud = strlen(mensaje);
		enviados = send(fd, mensaje, longitud, 0);
		if (enviados <= 0){
		    printf("Error al enviar datos\n");
		    exit(-1);
		}
		if((numbytes=recv(fd,buf,512,0)) == -1){
			printf("Error al recibir 2\n");
			exit(-1);
		}
messageText(buf);
		//ping
		sprintf(mensaje,"PING %d\r\n",canal);	
		longitud = strlen(mensaje);
		enviados = send(fd, mensaje, longitud, 0);
		if (enviados <= 0){
		    printf("Error al enviar datos\n");
		    exit(-1);
		}
		if((numbytes=recv(fd,buf,512,0)) == -1){
			printf("Error al recibir 2\n");
			exit(-1);
		}
	}else{
		sprintf(mensaje,"PRIVMSG %s :%s\r\n",canal,msg);	
		longitud = strlen(mensaje);
		enviados = send(fd, mensaje, longitud, 0);
		if (enviados <= 0){
		    printf("Error al enviar datos\n");
		    exit(-1);
		}
	}


	
	
	/*
	printf("%s\n",ptr);
	if(strstr(""ptr))
	ptr = strtok(NULL, "\r\n");
	printf("%s\n",ptr);*/

}


