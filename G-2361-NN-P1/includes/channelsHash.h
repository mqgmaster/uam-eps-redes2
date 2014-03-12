#include "usersHash.h"
#ifndef CHANNELSHASH_H
#define CHANNELSHASH_H

typedef struct {
	char *name;
	char *topic;
	User *users;
    struct UT_hash_handle hh; // handler
} Channel;

Channel* channelsHash_getAll();
Channel* channelsHash_put(char *name, char *topic);
void channelsHash_addUser(Channel *channel, User *user);
Channel* channelsHash_get(char *name);
void channelsHash_delete(char *name);
void channelsHash_deleteUser(Channel *channel, User *user);
void channelsHash_printLog();
int channelsHash_size();
int channelsHash_usersSize(Channel *channel);

#endif
