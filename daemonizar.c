#include "daemonizar.h"


void captura(int sennal){
	int status = 0;

	printf("funcion captura\n");
	wait(&status);
	closelog();	
	return;
}


int daeominzar (char * mensaje){
	pid_t pid;
	//1. Se capturan las señales SIGTTOU,SIGTTIN,SIGTSTP que provienen de la terminal con la funcion de C signal que enviara la captura a la funcion de tratamiento de señales predefinida SIG_IGN
	if(signal(SIGTTOU,SIG_IGN) == SIG_ERR || signal(SIGTTIN,SIG_IGN) == SIG_ERR || signal(SIGTSTP,SIG_IGN) == SIG_ERR){
		printf("Error al capturar las señales\n");	
		exit(ERROR);	
	}
	//2. Se crea un proceso hijo
	pid = fork();
	if(pid != 0){
		//3.Se cierra el proceso padre
		//padre
		exit(OK);
	}else{
		//4.Se crea una nueva sesion de tal forma que el proceso se convierte en el lider de la sesion
		openlog("sevidor",LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL3);
		syslog(LOG_INFO,"iniciando servidor.");
		if(setsid()<0){
			syslog(LOG_ERR,"Error al crear la sesion\n");
			exit(ERROR);
		}
		//5.Se pierde el control por parte del tty capturando la señal SIGHUP con la funcion SIG_IGN
		if(signal(SIGHUP,SIG_IGN)==SIG_ERR){
			printf("Error al caputrar la señal SIGHUP\n");
			exit(ERROR);	
		}
		//6.Los ficheros creados por el servidor deben ser accesibles a todo el mundo.Para ello es necesario cambiar la mascara de creacion de ficheros.
		//comprobar esto
		umask(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		//7.Por seguridad se puede cambiarel directorio de trabajo a, por ejemplo, en raiz
		if((chdir("/"))<0){
			printf("Error al cambiar el directorio de trabajo\n");
			exit(ERROR);
		}
		//8.Es necesario cerrar todos los ficheros abiertos previamente
		close(STDIN_FILENO); 
		close(STDOUT_FILENO); 
		close(STDERR_FILENO);
		//9. Se capturan las señales SIGCHLD y SIGPWR con la funcion que debe crearse que espera a que termine el demonio debido a estas señales y cierra el log;++
		if(signal(SIGCHLD,captura) == SIG_ERR || signal(SIGPWR,captura) == SIG_ERR){
			printf("Error al capturar las señales\n");	
			exit(ERROR);	
		}
		return OK;
	}
}
