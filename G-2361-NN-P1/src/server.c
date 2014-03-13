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


/*void captura(int sennal){
	int status = 0;

	printf("funcion captura\n");
	wait(&status);
	closelog();	
	return;
}*/

/*int daemonizar (char * mensaje){
	pid_t pid;
	/*1. Se capturan las señales SIGTTOU,SIGTTIN,SIGTSTP que provienen de la terminal con la funcion de C signal que enviara la captura a la funcion de tratamiento de señales predefinida SIG_IGN*/
/*	if(signal(SIGTTOU,SIG_IGN) == SIG_ERR || signal(SIGTTIN,SIG_IGN) == SIG_ERR || signal(SIGTSTP,SIG_IGN) == SIG_ERR){
		printf("Error al capturar las señales\n");	
		exit(ERROR);	
	}
	/*2. Se crea un proceso hijo*/
/*	pid = fork();
	if(pid != 0){
		/*3.Se cierra el proceso padre*/
/*		exit(OK);
	}else{
		/*4.Se crea una nueva sesion de tal forma que el proceso se convierte en el lider de la sesion*/
/*		if(setsid()<0){
			syslog(LOG_ERR,"Error al crear la sesion\n");
			exit(ERROR);
		}
/*		openlog(mensaje,LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL0);
		syslog(LOG_INFO,"iniciando servidor.");
		/*5.Se pierde el control por parte del tty capturando la señal SIGHUP con la funcion SIG_IGN*/
/*		if(signal(SIGHUP,SIG_IGN)==SIG_ERR){
			syslog(LOG_ERR,"Error al caputrar la señal SIGHUP\n");
			exit(ERROR);	
		}
		/*6.Los ficheros creados por el servidor deben ser accesibles a todo el mundo.Para ello es necesario cambiar la mascara de creacion de ficheros.*/
/*		umask(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		/*7.Por seguridad se puede cambiar el directorio de trabajo a, por ejemplo, en raiz*/
/*		if((chdir("/"))<0){
			syslog(LOG_ERR,"Error al cambiar el directorio de trabajo\n");
			exit(ERROR);
		}
		/*8.Es necesario cerrar todos los ficheros abiertos previamente*/
/*		close(STDIN_FILENO); 
		close(STDOUT_FILENO); 
		close(STDERR_FILENO);
		/*9. Se capturan las señales SIGCHLD y SIGPWR con la funcion que debe crearse que espera a que termine el demonio debido a estas señales y cierra el log;++*/
/*		if(signal(SIGCHLD,captura) == SIG_ERR || signal(SIGPWR,captura) == SIG_ERR){
			syslog(LOG_ERR,"Error al capturar las señales\n");	
			exit(ERROR);	
		}
		return OK;
	}
}*/

/*int openSocket(){
	return socket(AF_INET,SOCK_STREAM,0);
}

int assignSocket(int socketId,int numPort){
	infoS.sin_family=AF_INET;
	infoS.sin_port=htons(numPort);
	infoS.sin_addr.s_addr=INADDR_ANY;

	bzero(&(infoS.sin_zero), 8);	

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
	
int startListening(int socketIdc){
	int len;	
	int desc=0;
	len = sizeof(struct sockaddr_in);
	desc =  accept(socketIdc,(struct sockaddr *)&infoC,&len);
	if(desc<0){
		syslog(LOG_ERR,"Error al aceptar la conexion\n");
		return ERROR;
	}
	char clientIpString[INET_ADDRSTRLEN];
	int clientIpInt = infoC.sin_addr.s_addr;
	inet_ntop( AF_INET, &clientIpInt, clientIpString, INET_ADDRSTRLEN );
	syslog(LOG_INFO,"Se obtuvo una conexión desde %s\n", clientIpString);
	return desc;
}

int cerrarSesion(int socketId){
	return close(socketId);
}
*/
/*
int eviarDatos(const void ** msg, int longitud, int socketIdClient){
	int enviados=0;
	while (longitud > 0){
		enviados = send(socketIdClient, msg[socketIdClient], longitud, 0);
		if (enviados <= 0){
			syslog(LOG_ERR,"Error al enviar datos\n");
			return ERROR;
		}
		msg[socketIdClient] += enviados;
		longitud -= enviados;
	}
	return enviados;
}

int recibeDatos(int socketId, void ** msg){
	int recibidos=0;
	if(msg == NULL){
		syslog(LOG_ERR,"Error mensaje null\n");
		return ERROR;
	}
	recibidos = recv(socketId,(char*) msg[socketId], 500, 0);
	return recibidos;
}
*/


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
/*
int realizaAccion (int accion, int socketId, char *mensaje){
	char* ptr;
	int comprobacion=0;
	int i=0;
	char *command;
	char *message;
	Channel *channel;
	User *usuario;
	switch(accion){
		case CMD_NICK:
			usersHash_put(socketId, mensaje);
			usersHash_printLog();
			return OK;
		break;
		case CMD_USER:
		break;
		case CMD_QUIT:
			syslog(LOG_INFO,"cerrando sesion.....\n");
			if(cerrarSesion(socketId)==ERROR){
				syslog(LOG_ERR,"Error al cerrar sesion\n");
				return ERROR;
			}
			exit(OK);
			break;
		case CMD_PART:
		break;	
		case CMD_JOIN:
			syslog(LOG_INFO,"recibido mensaje: %s\n", mensaje);
			if(strstr(mensaje, "#")){
				channel = channelsHash_put(mensaje, "topic");
				usuario = usersHash_get(socketId);
				syslog(LOG_INFO,"canal creado %s %d %s\n", channel->name, socketId, usuario->nick);
				channelsHash_addUser(channel, usuario);
				return OK;
			}else{
				syslog(LOG_ERR,"Error en el formato del canal #<canal>%s\n", mensaje);
				return ERROR;
			}
		break;	
		case CMD_PRIVMSG:
			syslog(LOG_INFO,"list recibido mensaje: %s\n", mensaje);
			channelsHash_deleteUser(channelsHash_get("canal"), usersHash_get(1));
		break;
		case CMD_LIST:
			syslog(LOG_INFO,"list recibido mensaje: %s\n", mensaje);
			usersHash_printLog();
			usersHash_size();
			channelsHash_size();
			channelsHash_usersSize(channelsHash_get("#canal"));
			channelsHash_printLog();
			syslog(LOG_INFO,"test con iteraciones\n");
		 	User *testUser, *tmp;
			HASH_ITER(hh, usersHash_getAll(), testUser, tmp) {
		    	syslog(LOG_INFO,"usuario (%s)\n", testUser->nick);
			}			
		break;
		case CMD_PING:
		break;
		default:
			syslog(LOG_INFO,"recibido mensaje: %s\n", mensaje);
			syslog(LOG_INFO,"erro al realizar accion: \n");
	}
	return ERROR;
}

int nickFunction(char *mensaje,char**caracter, int socketId){
	
	char crlf[4]="\r\n";
	char* ptr, *mens;
	char inicio[8]="Server";
	int retorno=0,contador=0,i=0;	
	User * usuario;
	Channel * canal;
	
	ptr = strtok(mensaje," ");
	ptr = strtok(NULL," \r\n");
	syslog(LOG_INFO,"%s %s\n",mensaje,ptr);
	
	retorno = realizaAccion(CMD_NICK,socketId,ptr); 
	if(retorno == ERROR){
		return ERROR;
	}else{
		sprintf(caracter[socketId],":%s 001 %s :Welcome to the Internet Relay Network %s \r\n",servidor,ptr,ptr);
		if(eviarDatos((const void **) caracter, strlen(caracter[socketId]),socketId) == ERROR){
			syslog(LOG_ERR,"Error al enviar mensaje\n");
			return ERROR;	
		}
	}
	return OK;
}

int joinFunction(char *mensaje,char**caracter, int socketId){
	
	char crlf[4]="\r\n";
	char* ptr, *mens;
	char inicio[8]="Server";
	int retorno=0,contador=0,i=0;	
	User * usuario;
	Channel * canal;

	ptr = strtok(mensaje," ");
	ptr = strtok(NULL," \r\n");
	if(realizaAccion(CMD_JOIN,socketId,ptr)==ERROR){
		sprintf(caracter[socketId],":%s Error en el formato del canal #<canal> %s\r\n",servidor,ptr);
		return ERROR;
	}
	usuario = usersHash_get(socketId);
	canal = channelsHash_get(ptr);
	sprintf(caracter[socketId],":%s!%s JOIN :%s\r\n",usuario->nick,servidor,canal->name);
	if(eviarDatos((const void **) caracter, strlen(caracter[socketId]),socketId) == ERROR){
		syslog(LOG_ERR,"Error al enviar mensaje\n");
		return;	
	}
	sprintf(caracter[socketId],":%s 353 %s = %s :",servidor, usuario->nick,canal->name);		
	contador = channelsHash_usersSize(canal);
	
	for (i = 0; i < contador; i++){
		strcat(caracter[socketId],canal->users->nick);
		canal->users = canal->users->hh.next;
	}
	strcat(caracter[socketId],crlf);
	syslog(LOG_INFO,"%s\n",caracter[socketId]);
	channelsHash_printLog();

	if(eviarDatos((const void **) caracter, strlen(caracter[socketId]),socketId) == ERROR){
		syslog(LOG_ERR,"Error al enviar mensaje\n");
		return;	
	}

}

int privMsgFunction(char *mensaje,char**caracter, int socketId){
	char crlf[4]="\r\n";
	char* ptr,*canal,*mens;
	int retorno=0,contador=0,i=0;	
	Channel *chan;
	int chanSize;

	ptr = strtok(mensaje," ");
	canal = strtok(NULL," :");
	mens = strtok(NULL,"\r\n");
	syslog(LOG_INFO,"%s %s %s\n",ptr,canal,mens);

	chan = channelsHash_get(canal);
	chanSize=channelsHash_usersSize(chan);
	channelsHash_printLog();
	return OK;
}

int pingFunction(char *mensaje,char**caracter, int socketId){
	char* ptr;	

	ptr = strtok(mensaje," ");
	ptr = strtok(NULL," \r\n");
	sprintf(caracter[socketId],":%s PONG %s :%s",servidor, servidor,ptr);

	if(eviarDatos((const void **) caracter, strlen(caracter[socketId]),socketId) == ERROR){
		syslog(LOG_ERR,"Error al enviar mensaje\n");
		return ERROR;	
	}
	return OK;
}

int procesarMensaje (char * mensaje, char**caracter, int socketId){
	char crlf[4]="\r\n";
	char* ptr, *mens;
	char inicio[8]="Server";
	int retorno=0,contador=0,i=0;	
	User * usuario;
	Channel * canal;
	if(mensaje == NULL){
		syslog(LOG_ERR,"Error mensaje null\n");
		return ERROR;
	}
	if(strstr(mensaje,"NICK")!=NULL){
		if(nickFunction(mensaje,caracter,socketId)==ERROR){
			syslog(LOG_ERR,"ERROR en la funcion nickFunction\n");
			return ERROR;
		}
		return CMD_NICK;
	}else if(strstr(mensaje,"USER")!=NULL){
		//sprintf(caracter[socketId],"%s %s \r\n",inicio,mensaje);
		return CMD_USER;
	}else if(strstr(mensaje,"QUIT")!=NULL){
		ptr = strtok(mensaje," ");
		ptr = strtok(mensaje," ");
		if(ptr==NULL){
			sprintf(caracter[socketId],":%s QUIT \r\n",inicio);
		}else{
			sprintf(caracter[socketId],":%s QUIT :%s \r\n",inicio,ptr);
		}
		return CMD_QUIT;
	}else if(strstr(mensaje,"JOIN")!=NULL){
		if(joinFunction(mensaje,caracter,socketId)==ERROR){
			syslog(LOG_ERR,"ERROR en la funcion joinFunction\n");
			return ERROR;
		}
		//sprintf(caracter[socketId],":%s 366 %s %s \r\n",inicio, usuario->nick,canal->name);
		return CMD_JOIN;
	}else if(strstr(mensaje,"PRIVMSG")!=NULL){
		if(privMsgFunction(mensaje,caracter,socketId)==ERROR){
			syslog(LOG_ERR,"ERROR en la funcion nickFunction\n");
			return ERROR;
		}
		//sprintf(caracter[socketId],"%s \r\n",mensaje);
		return CMD_PRIVMSG;
	}else if(strstr(mensaje,"PASS")!=NULL){
		sprintf(caracter[socketId],"%s \r\n",mensaje);
		return CMD_PASS;
	}else if(strstr(mensaje,"LIST")!=NULL){
		if(realizaAccion(CMD_LIST,socketId,ptr)==ERROR){
			return ERROR;
		}
		sprintf(caracter[socketId],"%s \r\n",mensaje);
		return CMD_LIST;
	}else if(strstr(mensaje,"PING")!=NULL){
		if(pingFunction(mensaje,caracter,socketId)==ERROR){
			syslog(LOG_ERR,"ERROR en la funcion nickFunction\n");
			return ERROR;
		}
		//sprintf(caracter[socketId],"%s \r\n",mensaje);
		return CMD_PING;
	}
	return OK;
	
}*/

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

	//iniciarStructuras(canales);

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


