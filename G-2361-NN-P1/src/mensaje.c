#include "../includes/mensaje.h"

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
			/*usersHash_printLog();*/
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
			/*lista el contenido del hash.*/
			syslog(LOG_INFO,"list recibido mensaje: %s\n", mensaje);
			channelsHash_deleteUser(channelsHash_get("canal"), usersHash_get(1));
		break;
		case CMD_LIST:
			/*lista el contenido del hash.*/
			syslog(LOG_INFO,"list recibido mensaje: %s\n", mensaje);
			usersHash_printLog();
			usersHash_size();
			channelsHash_size();
			channelsHash_usersSize(channelsHash_get("#canal"));
			channelsHash_printLog();
			/* iteraciones en la hash: 
			* hh -> handler de las hash
			*/
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
		sprintf(caracter[socketId],":%s 001 %s :Welcome to the Internet Relay Network %s \r\n",inicio,ptr,ptr);
		if(eviarDatos((const void **) caracter, strlen(caracter[socketId]),socketId) == ERROR){
			syslog(LOG_ERR,"Error al enviar mensaje\n");
			return ERROR;	
		}
	}
	return OK;
}

int joinFunction(char *mensaje,char**caracter, int socketId){
	
	char crlf[4]="\r\n";
	char* ptr, **mens;
	char inicio[8]="Server";
	int retorno=0,contador=0,i=0;	
	User * usuario;
	Channel * canal;
	mens = (char**) calloc(512,sizeof(char*));

	ptr = strtok(mensaje," ");
	ptr = strtok(NULL," \r\n");
	if(realizaAccion(CMD_JOIN,socketId,ptr)==ERROR){
		sprintf(caracter[socketId],":%s Error en el formato del canal #<canal> %s\r\n",inicio,ptr);
		return ERROR;
	}
	usuario = usersHash_get(socketId);
	canal = channelsHash_get(ptr);
	sprintf(caracter[socketId],":%s!%s JOIN :%s\r\n",usuario->nick,inicio,canal->name);
	if(eviarDatos((const void **) caracter, strlen(caracter[socketId]),socketId) == ERROR){
		syslog(LOG_ERR,"Error al enviar mensaje\n");
		return;	
	}
	sprintf(caracter[socketId],":%s 353 %s = %s :",inicio, usuario->nick,canal->name);		
	contador = channelsHash_usersSize(canal);
	
	User *user, *tmp;
	HASH_ITER(hh, canal->users, user, tmp) {
    	strcat(caracter[socketId],user->nick);
    	strcat(caracter[socketId]," ");
	}
		
	strcat(caracter[socketId],crlf);
	syslog(LOG_INFO,"%s\n",caracter[socketId]);
	channelsHash_printLog();

	HASH_ITER(hh, canal->users, user, tmp) {
		mens[user->socketId] = (char*) calloc(512,sizeof(char));
		strcpy(mens[user->socketId],caracter[socketId]);
		syslog(LOG_INFO,"%s\n",mens[user->socketId]);
		if(eviarDatos((const void **) mens, strlen(caracter[socketId]),user->socketId) == ERROR){
				syslog(LOG_ERR,"Error al enviar mensaje\n");
				return;	
		}
	}
	

}

int privMsgFunction(char *mensaje,char**caracter, int socketId){
	char crlf[4]="\r\n";
	char* ptr,*canal,*mens,**mens1;
	int retorno=0,contador=0,i=0;	
	User * usuario;
	User *user, *tmp;
	Channel * chan;
	int chanSize;

	ptr = strtok(mensaje," ");
	canal = strtok(NULL," :");
	mens = strtok(NULL,"\r\n");
	syslog(LOG_INFO,"%s %s %s\n",ptr,canal,mens);
	usuario = usersHash_get(socketId);
	mens1 = (char**) calloc(512,sizeof(char*));
	if(strstr(canal,"#")!=NULL){
		//es un canal
		chan = channelsHash_get(canal);
		chanSize = channelsHash_usersSize(chan);

		HASH_ITER(hh, chan->users, user, tmp) {
			if (user->socketId != socketId) {
				mens1[user->socketId] = (char*) calloc(512,sizeof(char));
				sprintf(mens1[user->socketId],":%s PRIVMSG %s %s\r\n",usuario->nick,canal,mens);
				syslog(LOG_INFO,"%s\n",mens1[user->socketId]);
				if(eviarDatos((const void **) mens1, strlen(mens1[user->socketId]),user->socketId) == ERROR){
						syslog(LOG_ERR,"Error al enviar mensaje\n");
						return;
				}
			}
		}
	}else{
		//es un usuario
		user = usersHash_getByNick(canal);
		mens1[user->socketId] = (char*) calloc(512,sizeof(char));
		sprintf(mens1[user->socketId],":%s PRIVMSG %s %s\r\n",usuario->nick,user->nick,mens);
		syslog(LOG_INFO,"%s\n",mens1[user->socketId]);
		if(eviarDatos((const void **) mens1, strlen(mens1[user->socketId]),user->socketId) == ERROR){
				syslog(LOG_ERR,"Error al enviar mensaje\n");
				return;
		}		
	}

	return OK;
}

int pingFunction(char *mensaje,char**caracter, int socketId){
	char* ptr;	
	char inicio[]="Server";
	ptr = strtok(mensaje," ");
	ptr = strtok(NULL," \r\n");
	sprintf(caracter[socketId],":%s PONG %s :%s",inicio, inicio,ptr);

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
	
}