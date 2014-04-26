#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../includes/chat_funcs.h"
#include "../includes/sound.h"
  
int socketId;
int socketUdp,socketUdp1;
int puertoUdp;
char *ipUdp;
struct hostent *name;
struct sockaddr_in sound;
struct sockaddr_in sound1;
  
void procesaMensaje(char* mensaje, char* contestacion){
    char *mensaje1, *mensaje2;
    mensaje1 = strtok(mensaje, ":");
    while((mensaje2 = strtok(NULL, ":"))!=NULL){
        if(strstr(mensaje1,"PING")!=NULL){
            sprintf(contestacion, "PONG : %s\r\n",mensaje2);
        }
    }
}
  
void connectClient(void) {
    int puerto=-1;
    char *ip, *user, *nick, *nombre,*contestacion;  
    char buf[512], mensaje[512];
        
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
  
    if ((socketId=socket(AF_INET, SOCK_STREAM, 0))==-1){  
        printf("Error al abrir el socket\n");       
        exit(-1);
    }
  
    server.sin_family = AF_INET;
    server.sin_port = htons(puerto);
    server.sin_addr =*((struct in_addr *) he->h_addr);
    bzero(&(server.sin_zero),8);
      
    if(connect(socketId,(struct sockaddr *)&server, sizeof(struct sockaddr))==-1){
        errorText("Error al conectar con el servidor\n");       
        exit(-1);
    }
    receiveData(buf);
  
    messageText(buf);
  
    sprintf(mensaje, "NICK %s\r\nUSER %s %s %s :%s\r\n",nick,user,user, ip, nombre);
  
    sendData(mensaje);
    receiveData(buf);
    
    procesaMensaje(buf,contestacion);
    sendData(contestacion);
  
    startListeningThread(); 
}
  
void disconnectClient(void)
{
	send_quit();
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
	char* buffer = (char*) calloc (512, sizeof(char));
	strcpy(buffer, msg);
	if(strstr (buffer,"/m")!= NULL || strstr (buffer,"/privmsg")!= NULL || strstr (buffer,"/msg")!= NULL) {
		strtok(buffer," ");
		send_msg(buffer);
	}else if(strstr(buffer, "/pcall")!= NULL){
		send_sound(buffer);
	}else if(strstr(buffer, "/paccept")!= NULL){
		send_sound(buffer);
	}else if(strstr(buffer, "/pclose")!=NULL){		
		close(socketUdp);
		send_sound(buffer);
	}else if(strstr(buffer, "/join")!= NULL){
		send_join(buffer);
	}else if(strstr(buffer, "/part")!= NULL){
		send_part(buffer);
	}else if(strstr(buffer, "/nick")!= NULL){
		send_nick(buffer);
	}else if(strstr(buffer, "/quit")!= NULL){
		send_quit(buffer);
	}else if(strstr(buffer,"/kick")!=NULL){
		send_kick(buffer);	
	}else if(strstr(buffer,"/ban")!=NULL){
		send_kick(buffer);	
	}else if(strstr(buffer,"/mode")!= NULL){
		send_mode(buffer);
	}
	free(buffer);
	return;
}

//NICK
void send_nick(char* data){
	
	char *nick;
	char *buffer = (char*) calloc (512, sizeof(char));

	strtok(data," ");
	nick = strtok(NULL,"\r\n");
	if(nick==NULL){
		errorText("Uso: NICK <pseudónimo>, establece su pseudónimo");
	}else{
		sprintf(buffer,"NICK %s\r\n",nick);
		sendData(buffer);
	}
	free(buffer);
	return;
}

//PART
void send_part(char* data){
	char *canal;
	char *buffer = (char*) calloc (512, sizeof(char));
	strtok(data," ");
	canal = strtok(NULL,"\r\n");
	if(canal == NULL){
		errorText("Uso: PART <canal>");
	}else{
		sprintf(buffer,"PART %s :Saliendo\r\n",canal);
		sendData(buffer);
	}
	free(buffer);
	return;
}

//QUIT
void send_quit(){
	char *buffer = (char*) calloc (512, sizeof(char));
	sprintf(buffer, "QUIT :Saliendo\r\n");
	sendData(buffer);
	free(buffer);
	return;
}

//KICK
void send_kick(char * data){
	char *nick, *canal;
	char *buffer = (char*) calloc (512, sizeof(char));
	strtok(data," ");
	canal = strtok(NULL,"\r\n");
	nick = strtok(NULL,"\r\n");
	if(nick == NULL){
		errorText("Uso: KICK <canal> <nombre>");
	}else{
		sprintf(buffer,"KICK %s %s\r\n",canal, nick);
		sendData(buffer);
	}
	free(buffer);
	return;
}

//BAN
void send_ban(char * data){
	char *nick, *canal;
	char *buffer = (char*) calloc (512, sizeof(char));
	strtok(data," ");
	canal = strtok(NULL,"\r\n");
	nick = strtok(NULL,"\r\n");
	if(nick == NULL){
		errorText("Uso: BAN <canal> <nombre>");
	}else{
		sprintf(buffer,"BAN %s %s\r\n",canal, nick);
		sendData(buffer);
	}
	free(buffer);
	return;
}

//MODE
void send_mode(char * data){
	char *opcion, *canal, *user;
	char* buffer = (char*) calloc (512, sizeof(char));
	strtok(data," ");
	canal = strtok(NULL," ");
	opcion = strtok(NULL,"\r\n");
	
	if(strstr(opcion, "+o") != NULL){
		opcion = strtok(NULL," ");
		user = strtok(NULL,"\r\n");
		sprintf(buffer, "MODE %s +o %s\r\n",canal,user);
		sendData(buffer);
	}else if(strstr(opcion, "-o") != NULL){
		opcion = strtok(NULL," ");
		user = strtok(NULL,"\r\n");
		sprintf(buffer, "MODE %s +o %s\r\n",canal,user);
		sendData(buffer);
	}else if(strstr(opcion, "+p") != NULL){
		sprintf(buffer, "MODE %s +p\r\n",canal);
		sendData(buffer);
		sprintf(buffer, "MODE %s -s\r\n",canal);
		sendData(buffer);
	}else if(strstr(opcion, "-p") != NULL){
		sprintf(buffer, "MODE %s -p\r\n",canal);
		sendData(buffer);
	}else if(strstr(opcion, "+s") != NULL){
		sprintf(buffer, "MODE %s +s\r\n",canal);
		sendData(buffer);
		sprintf(buffer, "MODE %s -p\r\n",canal);
		sendData(buffer);
	}else if(strstr(opcion, "-s") != NULL){
		sprintf(buffer, "MODE %s -s\r\n",canal);
		sendData(buffer);
	}else if(strstr(opcion, "+t") != NULL){
		sprintf(buffer, "MODE %s +t\r\n",canal);
		sendData(buffer);
	}else if(strstr(opcion, "-t") != NULL){
		sprintf(buffer, "MODE %s -t\r\n",canal);
		sendData(buffer);
	}else if(strstr(opcion, "+m") != NULL){
		sprintf(buffer, "MODE %s +m\r\n",canal);
		sendData(buffer);
	}else if(strstr(opcion, "-m") != NULL){
		sprintf(buffer, "MODE %s -m\r\n",canal);
		sendData(buffer);
	}else if(strstr(opcion, "+n") != NULL){
		sprintf(buffer, "MODE %s +n\r\n",canal);
		sendData(buffer);
	}else if(strstr(opcion, "-n") != NULL){
		sprintf(buffer, "MODE %s -n\r\n",canal);
		sendData(buffer);
	}else if(strstr(opcion, "+l") != NULL){
		sprintf(buffer, "MODE %s +l\r\n",canal);
		sendData(buffer);
	}else if(strstr(opcion, "-l") != NULL){
		sprintf(buffer, "MODE %s -l\r\n",canal);
		sendData(buffer);
	}else if(strstr(opcion, "+k") != NULL){
		sprintf(buffer, "MODE %s +k\r\n",canal);
		sendData(buffer);
	}else if(strstr(opcion, "-k") != NULL){
		sprintf(buffer, "MODE %s -k\r\n",canal);
		sendData(buffer);
	}else if(strstr(opcion, "+i") != NULL){
		sprintf(buffer, "MODE %s +i\r\n",canal);
		sendData(buffer);
	}else if(strstr(opcion, "-i") != NULL){
		sprintf(buffer, "MODE %s -i\r\n",canal);
		sendData(buffer);
	}
}

//JOIN
void send_join(char* data){
	char *canal, comprobacion[100];
	char *buffer = (char*) calloc (512, sizeof(char));
	strtok(data," ");
	canal = strtok(NULL,"\r\n");
	if(strstr(canal,"#")!=NULL){
		sprintf(buffer,"JOIN %s\r\n",canal);
		sendData(buffer);
		
		sprintf(buffer,"MODE %s\r\n",canal);
		sendData(buffer);

		sprintf(buffer,"WHO %s\r\n",canal);
		sendData(buffer);	
		
		sprintf(buffer,"PING %s\r\n",canal);
		sendData(buffer);
		
		sprintf(comprobacion,"Canal: %s creado correctamente", canal);
		messageText(comprobacion);	
	}  
}
  
void sendData(const char *string) {
    int sentData = 0;
    int stringSize = strlen(string);
  
    while (stringSize > 0){
        sentData = send(socketId, string, stringSize, 0);
        if (sentData <= 0){
            errorText("Error al enviar datos");
            return;
        }
        stringSize -= sentData;
    }
}
  
void receiveData(char *msg){
    int received=0;
    received = recv(socketId,(char*) msg, 500, 0);
    if(msg == NULL){
        errorText("Error mensaje null");
        return;
    }
    msg[received] = '\0';
}
void send_sound(char * data){

	//char nombre[255];
	char *target,*buffer;
	struct ifaddrs *ifaddr, *ifa;
	int family, s;
	char host[NI_MAXHOST];

	if(strstr(data,"/pcall")!=NULL){
		socketUdp = socket (AF_INET, SOCK_DGRAM, 0);
		if (socketUdp == -1){  
			printf("Error al abrir el socket\n");       
			exit(-1);
		}

		if (getifaddrs(&ifaddr) == -1) {
			perror("getifaddrs");
			exit(EXIT_FAILURE);
		}

		for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
			if (ifa->ifa_addr == NULL)
			   continue;

			family = ifa->ifa_addr->sa_family;
			
			if (family == AF_INET) {
				if(strcmp(ifa->ifa_name,"eth0")==0){
					s = getnameinfo(ifa->ifa_addr,
					   (family == AF_INET) ? sizeof(struct sockaddr_in) :
								 sizeof(struct sockaddr_in6),
					   host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
					if (s != 0) {
						printf("getnameinfo() failed: %s\n", gai_strerror(s));
						exit(-1);
					}
					printf("address: <%s>\n", host);
				}
			}
		}
		sound.sin_family = AF_INET;
		sound.sin_port = htons(6666);
		sound.sin_addr.s_addr  = inet_addr(host);
		bzero(&(sound.sin_zero),8);

		if(bind(socketUdp,(struct sockaddr*)&sound,sizeof(struct sockaddr))==-1) {
			printf("error en bind() \n");
			exit(-1);
		} 

		startListeningThreadAudio();
		
		buffer = (char*) calloc (512, sizeof(char));
		target = strtok(data," ");
		target = strtok(NULL,"\r\n");
		if (target == NULL) {
			errorText("Formato no valido");
		}else if(strstr(target,"#")!=NULL){
			errorText("No se puede enviar auido a un canal");		
		}else{
			sprintf(buffer,"PRIVMSG %s :AUDIO %s %d %d\r\n",target,host,puertoUdp,socketUdp);
			sendData(buffer);
		}
		free(buffer);
		return;
	}else if(strstr(data,"/pclose")!= NULL){
		buffer = (char*) calloc (512, sizeof(char));
		target = strtok(data," ");
		target = strtok(NULL,"\r\n");
		if (target == NULL) {
			errorText("Formato no valido");
			return;
		}
		if(strstr(target,"#")!=NULL){
			errorText("No se puede enviar auido a un canal");		
		}else{
			sprintf(buffer,"PRIVMSG %s :NO PUEDO\r\n", target);
			sendData(buffer);
		}
		free(buffer);
		return;
	}else if(strstr(data,"/paccept")!= NULL){
		socketUdp1 = socket (AF_INET, SOCK_DGRAM, 0);
		if (socketUdp == -1){  
			printf("Error al abrir el socket\n");       
			exit(-1);
		}
		if (getifaddrs(&ifaddr) == -1) {
			perror("getifaddrs");
			exit(EXIT_FAILURE);
		}

		for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
			if (ifa->ifa_addr == NULL)
			   continue;

			family = ifa->ifa_addr->sa_family;
			
			if (family == AF_INET) {
				if(strcmp(ifa->ifa_name,"eth0")==0){
					s = getnameinfo(ifa->ifa_addr,
					   (family == AF_INET) ? sizeof(struct sockaddr_in) :
								 sizeof(struct sockaddr_in6),
					   host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
					if (s != 0) {
						printf("getnameinfo() failed: %s\n", gai_strerror(s));
						exit(-1);
					}
					printf("address: <%s>\n", host);
				}
			}
		}
		printf("Antes de asignar %d\n",socketUdp);
		sound1.sin_family = AF_INET;
		sound1.sin_port = htons(6666);
		sound1.sin_addr.s_addr  = inet_addr(host);
		bzero(&(sound1.sin_zero),8);

		printf("Despues de asignar\n");
		if(connect(socketUdp1,(struct sockaddr *)&sound1, sizeof(struct sockaddr)) == -1){
			printf("Error al conectar con el servidor\n");       
			exit(-1);
		}

		startListeningThreadAudio1();

		printf("Despues de asignar\n");
		buffer = (char*) calloc (512, sizeof(char));
		target = strtok(data," ");
		target = strtok(NULL,"\r\n");
		if (target == NULL) {
			errorText("Formato no valido");
		}
		if(strstr(target,"#")!=NULL){
			errorText("No se puede enviar auido a un canal");		
		}else{
			sprintf(buffer,"PRIVMSG %s :ACEPTO %s\r\n", target,host);
			sendData(buffer);
		}
		free(buffer);
		return;
	}
}
  


void send_msg(char* data) {
	char* target = strtok(NULL," ");
	char* text = strtok(NULL,"\r\n");
	char* buffer = (char*) calloc (512, sizeof(char));
	if (text == NULL || target == NULL) {
		errorText("Formato no valido");
		return;
	}
	if (strstr(target,"#") != NULL) {
		publicText(getApodo(), text);
	} else {
		privateText(getApodo(), text);
	}
	sprintf(buffer,"PRIVMSG %s :%s\r\n",target,text);
	sendData(buffer);
	free(buffer);
	return;
}
  
void receive_msg(char* data) {	
	char *source, *buffer, *pr, *pr1,*pr2;
	char msg[100],buf[256],buf2[256];
	socklen_t fromlen;
	if(strstr(data,"AUDIO")!= NULL){	
		source = strtok(data," ");
		strtok(NULL," ");
		strtok(NULL," ");
		strtok(NULL," ");
		pr = strtok(NULL," ");
		pr2 =strtok(NULL," ");
		pr1 = strtok(NULL,"\r\n");
		sprintf(msg,"¿Quieres recibir audio? del puerto: %s %s %s",pr,pr2,pr1);
		privateText(source, msg);
	}else if(strstr(data,"ACEPTO")!=NULL){
		source = strtok(data," ");
		//buf = strtok(NULL," ");
		buffer = (char*) calloc (512, sizeof(char));
		strcpy(msg,"Aceptado el audio. Empezando a grabar:...");
		privateText(source, msg);
    		sprintf(buffer,"PRIVMSG %s :EMPEZANDO\r\n",source);
		sendData(buffer);
		//audioReceiver();
		fromlen = sizeof(sound1);
		if(openRecord("prueba")) puts("error");
		if(openPlay("prueba")) puts("error");
		while(1){
			recordSound(buf,160);
			sendto(socketUdp, buf, strlen(buf), 0,(struct sockaddr*)&sound,sizeof(sound));
			
			recvfrom(socketUdp,buf2, strlen(buf2),0,(struct sockaddr*)&sound1, &fromlen);
			playSound(buf2,160);	
		}
		free(buffer);
	}else if(strstr(data,"EMPEZANDO")!=NULL){
		source = strtok(data," ");
		//buf = strtok(NULL," ");
		buffer = (char*) calloc (512, sizeof(char));
		strcpy(msg,"Aceptado el audio. Empezando a grabar:...");
		privateText(source, msg);
		fromlen = sizeof(sound);
		if(openPlay("prueba")) puts("error");
		if(openRecord("prueba")) puts("error");
		printf("Antes del wile1");
		while(1){
			recvfrom(socketUdp,buf2, strlen(buf2),0,(struct sockaddr*)&sound,&fromlen);
			playSound(buf2,160);

			recordSound(buf,160);
			sendto(socketUdp, buf, strlen(buf), 0,(struct sockaddr*)&sound1,sizeof(sound1));		
		}
	}else{
		source = strtok(data," ");
		strtok(NULL," ");
		strtok(NULL," ");
		buffer = strtok(NULL,"\r\n");
		removeChar(source,':');
		removeChar(msg,':');
		privateText(source, buffer);
	}
}
  
void sendAudio(const char *string){
	int sentData = 0;
	int stringSize = strlen(string);

	sentData = send(socketUdp, string, stringSize, 0);
	if (sentData <= 0){
	    errorText("Error al enviar el audio");
	    return;
	}
}

void recibeAudio(char *msg){
    recv(socketUdp,(char*) msg, 256, 0);
    if(msg == NULL){
        errorText("Error mensaje null");
        return;
    }
}
void startListeningThreadAudio1() {
    pthread_t thread_id;
    if(pthread_create(&thread_id,NULL,audioReceiver1,(void*) &socketUdp1) < 0){
        printf("No se ha podido crear el hilo.\n");
    }
}

void * audioReceiver1(){
	char buf[256],buf2[256];	
	socklen_t fromlen;
	if(openRecord("prueba")) puts("error");
	if(openPlay("prueba")) puts("error");
	while(1){
		recordSound(buf,160);
		sendto(socketUdp, buf, strlen(buf), 0,(struct sockaddr*)&sound,sizeof(sound));
		
		recvfrom(socketUdp,buf2, strlen(buf2),0,(struct sockaddr*)&sound1, &fromlen);
		playSound(buf2,160);	
	}	   
}

void startListeningThreadAudio() {
    pthread_t thread_id;
    if(pthread_create(&thread_id,NULL,audioReceiver,(void*) &socketUdp) < 0){
        printf("No se ha podido crear el hilo.\n");
    }
}

void * audioReceiver(){
	char buf[256],buf2[256];	
	socklen_t fromlen;
	if(openRecord("prueba")) puts("error");
	if(openPlay("prueba")) puts("error");
	while(1){
		recordSound(buf,160);
		sendto(socketUdp1, buf, strlen(buf), 0,(struct sockaddr*)&sound,sizeof(sound));
		
		recvfrom(socketUdp1,buf2, strlen(buf2),0,(struct sockaddr*)&sound1, &fromlen);
		playSound(buf2,160);	
	}	   
}

void startListeningThread() {
    pthread_t thread_id;
    if(pthread_create(&thread_id,NULL,messageReceiver,(void*) &socketId) < 0){
        printf("No se ha podido crear el hilo.\n");
    }
}
  
  
void* messageReceiver() {
    char* buffer = (char*) calloc (1024, sizeof(char));
    char* contestacion = (char*) calloc (512, sizeof(char));
    while(1) {
	
        receiveData(buffer);
  
	if (strstr (buffer,"PRIVMSG")!= NULL) {
		gdk_threads_enter();
		receive_msg(buffer);
		gdk_threads_leave();
        }else if(strstr (buffer,"PING")!= NULL){
		gdk_threads_enter();
		procesaMensaje(buffer,contestacion);
		sendData(contestacion);
		gdk_threads_leave();
	}else if(strstr(buffer,"443")!=NULL){
		printf("Erroooor\n");		
	}else if(strstr(buffer,"ERROR")!=NULL){
		close(socketId);
		return 0;
	}else{
		gdk_threads_enter();
		messageText(buffer);
		gdk_threads_leave();	
	}
    }
    return 0;
}
  
void removeChar(char *str, char garbage) {
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}
