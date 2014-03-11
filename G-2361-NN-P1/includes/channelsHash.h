#include "usersHash.h"
#ifndef CHANNELSHASH_H
#define CHANNELSHASH_H

typedef struct {
	char *name;
	User *users;
    struct UT_hash_handle hh; /* makes this structure hashable */
} Channel;

Channel* channelsHash_put(char *name);
int channelsHash_addUser(Channel *channel, User *user);
void channelsHash_printLog();
Channel* channelsHash_get(char *name);

#endif
