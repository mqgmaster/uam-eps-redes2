/*http://es.tldp.org/Tutoriales/PROG-SOCKETS/prog-sockets.html*/
/*https://gist.github.com/silv3rm00n/5821760*/
/*tail -f /var/log/syslog*/
/*http://www.binarytides.com/server-client-example-c-sockets-linux/*/
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
#include "../includes/daemon.h"
#include "../includes/socket.h"
#include "../includes/mensaje.h"

#define ERROR -1
#define MAXDATASIZE 100 
#define OK 0

struct sockaddr_in infoS, infoC;
int socketId;
char servidor[50]="";
typedef struct 
{
	int socketId;
	char * buffer;
}socketStruct;


void * conexionCliente(void *socket_desc){
	socketStruct sock = *(socketStruct*) socket_desc;
	char **mensaje;
	char *ptr;
	char ** contestacion;
	int read_size,valor;
	char limitador[] = "\r\n";

	mensaje = (char**) calloc(100,sizeof(char*));
	contestacion = (char**)calloc(100,sizeof(char*));
	mensaje[sock.socketId] = (char*) calloc(512,sizeof(char));
	contestacion[sock.socketId] = (char*) calloc(512,sizeof(char));

	usersHash_beginWrite();
	usersHash_put(sock.socketId, "");
	usersHash_endWrite();

	sprintf(contestacion[sock.socketId],":%s 020 * :Please wait wile we process your connection to %s\r\n",servidor,servidor);
	if(eviarDatos((const void **) contestacion, strlen(contestacion[sock.socketId]), sock.socketId) == ERROR){
		syslog(LOG_ERR,"Error al enviar mensaje\n");
		return NULL;	
	}

	if(read_size = recibeDatos(sock.socketId,(void **) mensaje)<0){
		syslog(LOG_ERR,"Error al recibir el mensaje\n");
		return NULL;
	}

	ptr = strtok(mensaje[sock.socketId], limitador);
	ptr = strtok(NULL, limitador); 
	syslog(LOG_INFO,"%s %s\n",mensaje[sock.socketId],ptr);

	procesarMensaje(mensaje[sock.socketId],contestacion,sock.socketId);
	procesarMensaje(ptr,contestacion,sock.socketId);
	strcpy(mensaje[sock.socketId], "");
	syslog(LOG_INFO,"%s\n",mensaje[sock.socketId]);
	while( (read_size = recibeDatos(sock.socketId,(void **) mensaje)) > 0 ){
		syslog(LOG_INFO,"%s \n",mensaje[sock.socketId]);
		ptr = strtok(mensaje[sock.socketId], limitador);   
		procesarMensaje(mensaje[sock.socketId],contestacion,sock.socketId);
		valor = strlen(contestacion[sock.socketId]);
		if(eviarDatos((const void **) contestacion, valor, sock.socketId) == ERROR){
			syslog(LOG_ERR,"Error al enviar mensaje\n");
			return NULL;	
		}
	}
	return OK;
}

int initServer(char * servidor,int numPort,int longMax){

	if(daemonizar(servidor)==ERROR){
		printf("Error al daemonizar\n");
		return ERROR;
	}

	if((socketId = openSocket())<0){
		printf("Error al crear el socket\n");
		exit(ERROR);
	}
	if(assignSocket(socketId,numPort)<0){
		printf("Error al asignar el puerto al socket\n");
		exit(ERROR);
	}

	//abrira la cola de procesos
	if(openQueue(socketId,longMax) !=0){
		printf("Error al abrir la cola\n");
		exit(ERROR);
	}
	return OK;
}

int main(int argc, char *argv[]){
	int numPort,longMax,socketIdC,i=0,j=0;
	socketStruct *socketStC;
	pthread_t thread_id[100];
	/***/
	int valor,proc;
	char * retorno;
	char **mensaje;
	int read_size;
	int len;
	char limitador[2] = "\r\n";
	char *ptr;
	char ** contestacion;
	/***/
	
	if(argc != 4){
		printf("Error en el numero de parametros\n");
		return ERROR;
	}

	usersHash_init();
	channelsHash_init();

	numPort = atoi(argv[2]);
	longMax = atoi(argv[3]);
	strcpy(servidor,argv[1]);
	if(initServer(argv[1],numPort,longMax)==ERROR){
		syslog(LOG_ERR,"Error al abrir el servidor\n");		
		exit(ERROR);
	}

	i=0;
	syslog(LOG_INFO,"Esperando conexion\n");
	socketStC = (socketStruct*) calloc(512,sizeof(socketStruct));
	while(1){
		socketIdC = startListening(socketId);
		if(socketIdC < 0){
			syslog(LOG_ERR,"Error al conectar con el cliente \n");
			return ERROR;
		}
		socketStC[socketIdC].socketId = socketIdC;
		socketStC[socketIdC].buffer = calloc (50,sizeof(char));

		if(pthread_create(&thread_id[i],NULL,conexionCliente,(void*) &socketStC[socketIdC]) < 0){
			printf("No se ha podido crear el hilo.\n");
			return ERROR;
       		}
		i++;
	}	
	return OK;
}


