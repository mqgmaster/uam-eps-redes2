#include "../includes/usersHash.h"
#include "../includes/uthash.h"
#include "../includes/boolean.h"
#include <stdio.h>
#include <syslog.h>

User *usersHash = NULL;     

User* usersHash_put(char *nick, int socketId) {       
	User *user;
	HASH_FIND_STR(usersHash, nick, user);
	if (user)
	{
		return user;                    
	}
	user = malloc(sizeof(User));
	user->nick = malloc(strlen(nick) + 1);
	strcpy(user->nick, nick);
	user->socketId = socketId;
	HASH_ADD_KEYPTR(hh, usersHash, user->nick, strlen(user->nick), user);
	return user;
}

int usersHash_exists(char *nick) {       
	User *user;
	HASH_FIND_STR(usersHash, nick, user);
	if (user) {
		return TRUE;                    
	} else {
		return FALSE;
	}
	
}

User* usersHash_get(char *nick)
{
	User *user;
	HASH_FIND_STR(usersHash, nick, user);
	return user;
}

void usersHash_delete(char *nick) {
	User *user;
	HASH_FIND_STR(usersHash, nick, user);
	if (user) {
		syslog(LOG_INFO,"usuario do nick(%s) removido\n", user->nick);
		HASH_DEL(usersHash, user);                 
	} 
}

void usersHash_printLog()
{       
	User *user;
	for (user = usersHash; user != NULL; user = user->hh.next) {
		syslog(LOG_INFO,"nick(%s) socketId(%d)\n", user->nick, user->socketId);
	}
}
