#include "uthash.h"
#ifndef USERSHASH_H
#define USERSHASH_H

typedef struct {
	char *nick;
	int socketId;
	//id da thread?
    struct UT_hash_handle hh; /* makes this structure hashable */
} User;

User* usersHash_put(char *nick, int socketId);
void usersHash_dump();
User* usersHash_get(char *nick);
int usersHash_exists(char *nick);

#endif
