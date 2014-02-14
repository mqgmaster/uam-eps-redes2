 
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

#define ERROR -1
#define OK 0

struct sockaddr_in infoS, infoC;

int openSocket(){
//TCP cambiado por 0
	return socket(AF_INET,SOCK_STREAM,0);
}

int assignSocket(int socketId,int numPort){
	//TCP/IP
	infoS.sin_family=AF_INET;
	// asignamos el puerto
	infoS.sin_port=htons(numPort);
	//aceptar todas las direcciones
	infoS.sin_addr.s_addr=INADDR_ANY;
	
	
	//mirar esta funcion	
	bzero(&(infoS.sin_zero), 8);	

	// asignar el puerto al soket
	if(bind(socketId,(struct sockaddr *)&infoS,sizeof(struct sockaddr)) == ERROR){
		printf("Error al asignar el socket, %s\n" ,strerror(errno));
		return ERROR;
	}
	return OK;
}

int openQueue(int socketId, int longQueue){
	return listen(socketId,longQueue);
}

int closeServer(int socketId){
	close(socketId);
	return OK;	
}

int startListening(int socketId){
	int len;		
	len = sizeof(infoC);
	return accept(socketId,(struct sockaddr *)&infoC,&len);
}



int main(int argc, char *argv[]){
	int numPort,longMax,socketId,socketIdC;
	
	if(argc != 3){
		printf("Error en el numero de parametros\n");
		return ERROR;
	}

	numPort = atoi(argv[1]);
	longMax = atoi(argv[2]);

	//creamos el socket, no sabemos que protocolo se usa, TCP o ponemos un 0 como en las transp.???? probar
	
	if((socketId = openSocket())<0){
		printf("Error al crear el socket\n");
		exit(ERROR);
	}
	printf("%d, %d, %d \n",socketId,numPort,longMax);
	if(assignSocket(socketId,numPort)<0){
		printf("Error al asignar el puerto al socket\n");
		exit(ERROR);
	}

	//abrira la cola de procesos
	if(openQueue(socketId,longMax) !=0){
		printf("Error al abrir la cola\n");
		exit(ERROR);
	}
	
	//El servidor se pondra a escuchar peticiones
	while(1){
		if(socketIdC=startListening(socketId) == ERROR){
			printf("Error al aceptar\n");
			return ERROR;
		}
	}	

	return OK;
}
