#include "uthash.h"
#include "boolean.h"
#include <stdio.h>
#include <syslog.h>
#ifndef USERSHASH_H
#define USERSHASH_H

typedef struct {
	int socketId;
	char *nick;
	//id da thread?
    struct UT_hash_handle hh; // handler
} User;

User* usersHash_getAll();
User* usersHash_put(int socketId, char *nick);
User* usersHash_get(int socketId);
User* usersHash_getByNick(char *nick);
void usersHash_delete(int socketId);
void usersHash_printLog(); 
int usersHash_size();

User* usersHash_put_(User **usersHash, int socketId, char *nick);
User* usersHash_get_(User **usersHash, int socketId);
User* usersHash_getByNick_(User **usersHash, char *nick);
void usersHash_delete_(User **usersHash, int socketId);
void usersHash_printLog_(User **usersHash); 
int usersHash_size_(User **hash);

#endif
