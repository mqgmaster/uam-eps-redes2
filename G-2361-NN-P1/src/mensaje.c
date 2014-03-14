#include "../includes/mensaje.h"

char crlf[]="\r\n";
char inicio[]="Server";

int eviarDatos(const void ** msg, int longitud, int socketIdClient){
	int enviados=0;
	usersHash_beginRead();
	User *user = usersHash_get(socketIdClient);
	userSocket_beginWrite(user); 
	while (longitud > 0){
		enviados = send(user->socketId, msg[user->socketId], longitud, 0);
		if (enviados <= 0){
			syslog(LOG_ERR,"Error al enviar datos\n");
			return ERROR;
		}
		msg[user->socketId] += enviados;
		longitud -= enviados;
	}
	userSocket_endWrite(user); 
	usersHash_endRead();
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
		break;
		case CMD_USER:
		break;
		case CMD_QUIT:
		break;
		case CMD_PART:
		break;	
		case CMD_JOIN:
			syslog(LOG_INFO,"recibido mensaje: %s\n", mensaje);
			if(strstr(mensaje, "#")){
				usersHash_beginRead();
				channelsHash_beginWrite();

				channelsHash_put(mensaje, "topic");

				channel = channelsHash_get(mensaje);
				usuario = usersHash_get(socketId);
				syslog(LOG_INFO,"canal creado %s %d %s\n", channel->name, socketId, usuario->nick);
				channelsHash_addUser(channel, usuario);

				channelsHash_endWrite();
				usersHash_endRead();
				return OK;
			}else{
				syslog(LOG_ERR,"Error en el formato del canal #<canal>%s\n", mensaje);
				return ERROR;
			}
		break;	
		case CMD_PRIVMSG:
			/*lista el contenido del hash.*/
			syslog(LOG_INFO,"list recibido mensaje: %s\n", mensaje);
		break;
		case CMD_LIST:
			/*lista el contenido del hash.*/
			syslog(LOG_INFO,"list recibido mensaje: %s\n", mensaje);
			usersHash_beginRead();
			channelsHash_beginRead();

			usersHash_printLog();
			usersHash_size();
			channelsHash_size();
			channelsHash_printLog();

			usersHash_endRead();
			channelsHash_endRead();
		break;
		case CMD_PING:
		break;
		default:
			syslog(LOG_INFO,"recibido mensaje: %s\n", mensaje);
			syslog(LOG_INFO,"erro al realizar accion: \n");
	}
	return ERROR;
}

int cmd_nick(char *mensaje,char**caracter, int socketId){
	char* action, *nick;
	char *message,*previousNick;
	message = (char*) calloc(512,sizeof(char));
	previousNick = (char*) calloc(50,sizeof(char));

	action = strtok(mensaje," ");
	nick = strtok(NULL," ");
	syslog(LOG_INFO,"%s %s\n",mensaje,nick);
	if (nick == NULL) {
		sprintf(message,":%s %d %s :No nickname given%s",inicio,ERR_NONICKNAMEGIVEN,nick,crlf);
		sendData(message, socketId);
		return OK;
	} 

	usersHash_beginRead();
	User *user = usersHash_getByNick(nick);
	if (user) {
		if (user->socketId != socketId) {
			sprintf(message,":%s %d %s :Nickname is already in use%s",inicio,ERR_NICKNAMEINUSE,nick,crlf);
			sendData(message, socketId);
			usersHash_endRead();
			return OK;
		}
	}
	usersHash_endRead();

	usersHash_beginWrite();
	User *currentUser = usersHash_get(socketId);
	strcpy(previousNick,currentUser->nick);
	usersHash_put(socketId, nick);
	usersHash_endWrite();

	usersHash_beginRead();
	usersHash_printLog();

	if (strcmp(previousNick, "temp") != 0) {
		sprintf(message,":%s!~%s NICK %s%s",previousNick,previousNick,nick,crlf);
		syslog(LOG_INFO,"%s\n",message);
		User *aUser, *tmp;
		HASH_ITER(hh, usersHash_getAll(), aUser, tmp) {
			if(sendData(message, aUser->socketId) == ERROR){
				syslog(LOG_ERR,"Error al enviar mensaje\n");
				return;	
			}
		}
	}
	
	sprintf(caracter[socketId],":%s 001 %s :Welcome to the Internet Relay Network %s \r\n",inicio,nick,nick);
	if(eviarDatos((const void **) caracter, strlen(caracter[socketId]),socketId) == ERROR){
		syslog(LOG_ERR,"Error al enviar mensaje\n");
		return ERROR;	
	}
	usersHash_endRead();
	return OK;
}

int cmd_join(char *mensaje,char**caracter, int socketId){
	char* ptr, **mens;
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
	usersHash_beginRead();
	channelsHash_beginRead();

	usuario = usersHash_get(socketId);
	canal = channelsHash_get(ptr);
	sprintf(caracter[socketId],":%s!%s JOIN :%s\r\n",usuario->nick,inicio,canal->name);
	if(eviarDatos((const void **) caracter, strlen(caracter[socketId]),socketId) == ERROR){
		syslog(LOG_ERR,"Error al enviar mensaje\n");
		return;	
	}
	sprintf(caracter[socketId],":%s 353 %s = %s :",inicio, usuario->nick,canal->name);		
	
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
	
	usersHash_endRead();
	channelsHash_endRead();
}

int cmd_privmsg(char *mensaje,char**caracter, int socketId){
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

	usersHash_beginRead();

	usuario = usersHash_get(socketId);

	mens1 = (char**) calloc(512,sizeof(char*));
	if(strstr(canal,"#")!=NULL){
		//es un canal
		channelsHash_beginRead();

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
		channelsHash_endRead();

	}else{
		//es un usuario
		user = usersHash_getByNick(canal);
		if (user) {
			mens1[user->socketId] = (char*) calloc(512,sizeof(char));
			sprintf(mens1[user->socketId],":%s PRIVMSG %s %s\r\n",usuario->nick,user->nick,mens);
			syslog(LOG_INFO,"%s\n",mens1[user->socketId]);
			if(eviarDatos((const void **) mens1, strlen(mens1[user->socketId]),user->socketId) == ERROR){
					syslog(LOG_ERR,"Error al enviar mensaje\n");
					return;
			}
		}
	}

	usersHash_endRead();
	return OK;
}

int cmd_ping(char *mensaje,char**caracter, int socketId){
	char* ptr;	
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
	char* ptr;

	if(mensaje == NULL){
		syslog(LOG_ERR,"Error mensaje null\n");
		return ERROR;
	}
	if(strstr(mensaje,"NICK")!=NULL){
		if(cmd_nick(mensaje,caracter,socketId)==ERROR){
			syslog(LOG_ERR,"ERROR en la funcion nickFunction\n");
			return ERROR;
		}
		return CMD_NICK;
	} else if(strstr(mensaje,"USER")!=NULL){
		return CMD_USER;
	} else if(strstr(mensaje,"QUIT")!=NULL){
		if(cmd_quit(mensaje,caracter,socketId)==ERROR){
			syslog(LOG_ERR,"ERROR en la funcion cmd_quit\n");
			return ERROR;
		}
		return CMD_QUIT;
	} else if(strstr(mensaje,"JOIN")!=NULL){
		if(cmd_join(mensaje,caracter,socketId)==ERROR){
			syslog(LOG_ERR,"ERROR en la funcion joinFunction\n");
			return ERROR;
		}
		return CMD_JOIN;
	} else if(strstr(mensaje,"PRIVMSG")!=NULL){
		if(cmd_privmsg(mensaje,caracter,socketId)==ERROR){
			syslog(LOG_ERR,"ERROR en la funcion cmd_privmsg\n");
			return ERROR;
		}
		return CMD_PRIVMSG;
	} else if(strstr(mensaje,"PART")!=NULL){
		if(cmd_part(mensaje,caracter,socketId)==ERROR){
			syslog(LOG_ERR,"ERROR en la funcion cmd_part\n");
			return ERROR;
		}
		return CMD_PART;
	} else if(strstr(mensaje,"PASS")!=NULL){
		sprintf(caracter[socketId],"%s \r\n",mensaje);
		return CMD_PASS;
	} else if(strstr(mensaje,"LIST")!=NULL){
		if(cmd_list(mensaje,caracter,socketId)==ERROR){
			return ERROR;
		}
		sprintf(caracter[socketId],"%s \r\n",mensaje);
		return CMD_LIST;
	} else if(strstr(mensaje,"NAMES")!=NULL){
		if(cmd_names(mensaje,caracter,socketId)==ERROR){
			return ERROR;
		}
		sprintf(caracter[socketId],"%s \r\n",mensaje);
		return CMD_NAMES;
	} else if(strstr(mensaje,"PING")!=NULL){
		if(cmd_ping(mensaje,caracter,socketId)==ERROR){
			syslog(LOG_ERR,"ERROR en la funcion nickFunction\n");
			return ERROR;
		}
		return CMD_PING;
	}
	return OK;
	
}

int cmd_list(char *msg,char**caracter, int socketId) {
	char *action,*params,*response;

	Channel *channel;

	action = strtok(msg," ");
	params = strtok(NULL,crlf);
	syslog(LOG_INFO,"%s %s\n",action,params);
	response = (char*) calloc(512,sizeof(char));

	channelsHash_beginRead();

	if(params != NULL && strstr(params,"#")!=NULL) {
		channel = channelsHash_get(params);

		if (channel) {
			sprintf(response,"%s :%s%s",channel->name,channel->topic,crlf);
			syslog(LOG_INFO,"%s\n",response);
			if(sendData(response, socketId) == ERROR){
					syslog(LOG_ERR,"Error al enviar mensaje\n");
					return;
			}
		}
	} else {
		Channel *tmp;
		HASH_ITER(hh, channelsHash_getAll(), channel, tmp) {
			sprintf(response,"%s :%s%s",channel->name,channel->topic,crlf);
			syslog(LOG_INFO,"%s\n",response);
			if(sendData(response, socketId) == ERROR){
					syslog(LOG_ERR,"Error al enviar mensaje\n");
					return;
			}
		}
	}

	channelsHash_endRead();

	return OK;
}

int cmd_names(char *msg, char**caracter, int socketId) {
	char *action,*params,*response;
	Channel *channel;

	action = strtok(msg," ");
	params = strtok(NULL,crlf);
	syslog(LOG_INFO,"%s %s\n",action,params);
	response = (char*) calloc(512,sizeof(char));

	channelsHash_beginRead();

	if(params != NULL && strstr(params,"#")!=NULL) {
		channel = channelsHash_get(params);

		if (channel) {
			sprintf(response,"%s :",channel->name);

			User *user, *tmp;
			HASH_ITER(hh, channel->users, user, tmp) {
				strcat(response,user->nick);
    			strcat(response," ");
    		}
				
    		strcat(response,crlf);
				
			syslog(LOG_INFO,"%s\n",response);
			if(sendData(response, socketId) == ERROR){
				syslog(LOG_ERR,"Error al enviar mensaje\n");
				return;
			}
		}
	} else {
		Channel *tmp;
		HASH_ITER(hh, channelsHash_getAll(), channel, tmp) {
			sprintf(response,"%s :",channel->name);

			User *user, *tmp;
			HASH_ITER(hh, channel->users, user, tmp) {
				strcat(response,user->nick);
    			strcat(response," ");
    		}
    		strcat(response,crlf);

			syslog(LOG_INFO,"%s\n",response);
			if(sendData(response, socketId) == ERROR){
					syslog(LOG_ERR,"Error al enviar mensaje\n");
					return;
			}
		}
	}

	channelsHash_endRead();

	return OK;
}

int cmd_quit(char *msg, char**caracter, int socketId) {
	char *message;
	message = (char*) calloc(512,sizeof(char));
	
	channelsHash_beginRead();
	usersHash_beginRead();

	User *userExiting = usersHash_get(socketId);
	sprintf(message,":%s!~%s QUIT :Adios%s",userExiting->nick,userExiting->nick,crlf);
	syslog(LOG_INFO,"%s\n",message);

	Channel *channel;
	User *user;
	for (channel = channelsHash_getAll(); channel != NULL; channel = channel->hh.next) {
		if (channelsHash_existsUser(channel, userExiting) == TRUE) {
			for (user = channel->users; user != NULL; user = user->hh.next) {
				if(user->socketId != userExiting->socketId) {
					if(sendData(message, user->socketId) == ERROR){
						syslog(LOG_ERR,"Error al enviar mensaje\n");
						return;	
					}
				}
			}
		}
	}

	channelsHash_endRead();
	usersHash_endRead();

	channelsHash_beginWrite();
	usersHash_beginWrite();

	usersHash_delete(userExiting->socketId);
	channelsHash_deleteUserFromAll(userExiting);

	usersHash_printLog();

	channelsHash_endWrite();
	usersHash_endWrite();

	syslog(LOG_INFO,"cerrando sesion.....\n");
	if(cerrarSesion(socketId)==ERROR){
		syslog(LOG_ERR,"Error al cerrar sesion\n");
		return ERROR;
	}

	pthread_exit(OK);
}

int cmd_part(char *msg, char**caracter, int socketId) {
	char *action,*params,*response;
	Channel *channel;
	User *userExiting;
	action = strtok(msg," ");
	params = strtok(NULL," ");
	syslog(LOG_INFO,"%s %s\n",action,params);
	response = (char*) calloc(512,sizeof(char));

	channelsHash_beginRead();

	if(params != NULL && strstr(params,"#")!=NULL) {
		channel = channelsHash_get(params);

		userExiting = usersHash_get(socketId);
		if (channelsHash_existsUser(channel, userExiting) == TRUE) {
			sprintf(response,":%s!~%s PART %s%s",userExiting->nick,userExiting->nick,channel->name,crlf);
			syslog(LOG_INFO,"%s\n",response);

			User *user,*tmp;
			HASH_ITER(hh, channel->users, user, tmp) {
				if(sendData(response, user->socketId) == ERROR){
					syslog(LOG_ERR,"Error al enviar mensaje\n");
					return;	
				}
    		}
		}
	}

	channelsHash_endRead();

	channelsHash_beginWrite();
	usersHash_beginWrite();

	channelsHash_deleteUser(channel, userExiting);

	channelsHash_printLog();

	channelsHash_endWrite();
	usersHash_endWrite();

	return OK;
}

int sendData(const char *string, int socketId) {
	int sentData = 0;
	int stringSize = strlen(string);

	usersHash_beginRead();
	User *user = usersHash_get(socketId);
	userSocket_beginWrite(user); 

	while (stringSize > 0){
		sentData = send(user->socketId, string, stringSize, 0);
		if (sentData <= 0){
			syslog(LOG_ERR,"Error al enviar datos\n");
			return ERROR;
		}
		stringSize -= sentData;
	}

	userSocket_endWrite(user); 
	usersHash_endRead();
	return sentData;
}
