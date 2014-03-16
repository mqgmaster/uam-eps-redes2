#include "../includes/socket.h"

int openSocket(){
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

int cerrarSesion(int socketId){
	return close(socketId);
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