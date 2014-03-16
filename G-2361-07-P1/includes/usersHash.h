#include "uthash.h"
#include "boolean.h"
#include <stdio.h>
#include <syslog.h>
#include <pthread.h>
#ifndef USERSHASH_H
#define USERSHASH_H

//*atencion*//
//la estructura no es thread-safe
//mirar las observaciones abajo
typedef struct {
	int socketId;
	char *us;
	char *nick;
	pthread_mutex_t socketLock;
    struct UT_hash_handle hh; // handler
} User;

//PUBLIC FUNCTIONS
//no thread-safe
//OBLIGATORIO uso de beginRead / beginWrite
User* usersHash_getAll();
User* usersHash_get(int socketId);
User* usersHash_getByNick(char *nick);
void usersHash_put(int socketId, char *nick);
void usersHash_delete(int socketId);
void usersHash_printLog(); 
int usersHash_size();

//control de threads
void usersHash_beginRead();
void usersHash_endRead();
void usersHash_beginWrite();
void usersHash_endWrite();

//funciones de control del uso del socket
void userSocket_beginWrite(User *user);
void userSocket_endWrite(User *user);

void usersHash_init();

//END PUBLIC FUNCTIONS

//funciones internas, de uso exclusivo de otros modulos hash
void usersHash_put_(User **usersHash, int socketId, char *nick);
User* usersHash_get_(User **usersHash, int socketId);
User* usersHash_getByNick_(User **usersHash, char *nick);
void usersHash_delete_(User **usersHash, int socketId);
void usersHash_printLog_(User **usersHash); 
int usersHash_size_(User **hash);

#endif
