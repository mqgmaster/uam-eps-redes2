#include "usersHash.h"
#ifndef CHANNELSHASH_H
#define CHANNELSHASH_H

//*atencion*//
//la estructura no es thread-safe
//mirar las observaciones abajo
typedef struct {
	char *name;
	char *topic;
	User *users;
    struct UT_hash_handle hh; // handler
} Channel;

//PUBLIC FUNCTIONS
//no thread-safe
//OBLIGATORIO uso de beginRead / beginWrite
Channel* channelsHash_getAll();
Channel* channelsHash_get(char *name);
void channelsHash_put(char *name, char *topic);
void channelsHash_addUser(Channel *channel, User *user);
void channelsHash_delete(char *name);
void channelsHash_deleteUser(Channel *channel, User *user);
void channelsHash_deleteUserFromAll(User *user);
void channelsHash_printLog();
int channelsHash_size();
int channelsHash_usersSize(Channel *channel);

//control de threads
void channelsHash_beginRead();
void channelsHash_endRead();
void channelsHash_beginWrite();
void channelsHash_endWrite();

void channelsHash_init();

#endif
