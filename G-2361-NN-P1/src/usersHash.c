#include "../includes/usersHash.h"

User *usersHash = NULL;
pthread_rwlock_t usersHash_lock;

void usersHash_init() {
	if (pthread_rwlock_init(&usersHash_lock,NULL) != 0) syslog(LOG_INFO,"error al crear lock\n");
}

User* usersHash_getAll() {
	return usersHash;
}

void usersHash_put(int socketId, char *nick) {
	usersHash_put_(&usersHash, socketId, nick);
}

User* usersHash_get(int socketId) {
	return usersHash_get_(&usersHash, socketId);
}

void usersHash_delete(int socketId) {
	usersHash_delete_(&usersHash, socketId);
}

int usersHash_size() {
	return usersHash_size_(&usersHash);
}

void usersHash_printLog() {
	usersHash_printLog_(&usersHash);
}

User* usersHash_getByNick(char *nick) {
	return usersHash_getByNick_(&usersHash, nick);
}

void usersHash_put_(User **hash, int socketId, char *nick) {       
	User *user;
	HASH_FIND_INT(*hash, &socketId, user);
	if (user) {
		strcpy(user->nick, nick); 
		return;                    
	}
	user = malloc(sizeof(User));
	user->us = malloc(50);
	user->nick = malloc(strlen(nick) + 1);
	strcpy(user->nick, nick);
	user->socketId = socketId;
	if (pthread_mutex_init(&(user->socketLock),NULL) != 0) syslog(LOG_INFO,"error al init mutex\n");
	syslog(LOG_INFO,"creando usuario\n");
	HASH_ADD_INT(*hash, socketId, user);
}

User* usersHash_get_(User **hash, int socketId) {
	User *user;
	HASH_FIND_INT(*hash, &socketId, user);
	return user;
}

User* usersHash_getByNick_(User **hash, char *nick) {
	User *user;
	for (user = *hash; user != NULL; user = user->hh.next) {
		if(strcmp(user->nick, nick) == 0) {
			return user;
		}
	}
}

void usersHash_delete_(User **hash, int socketId) {
	User *user;
	HASH_FIND_INT(*hash, &socketId, user);
	if (user) {
		syslog(LOG_INFO,"usuario do nick(%s) removido\n", user->nick);
		HASH_DEL(*hash, user);                 
	} 
}

void usersHash_printLog_(User **hash) {  
	syslog(LOG_INFO,"users print\n");
	User *user;
	for (user = *hash; user != NULL; user = user->hh.next) {
		syslog(LOG_INFO,"nick(%s) socketId(%d)\n", user->nick, user->socketId);
	}
}

int usersHash_size_(User **hash) {
	int size = HASH_COUNT(*hash);
	syslog(LOG_INFO,"usuarios (hash size): %d\n", size);
	return size;
}

void usersHash_beginRead() {
	if (pthread_rwlock_rdlock(&usersHash_lock) != 0) syslog(LOG_INFO,"error al begin read lock\n");
}

void usersHash_endRead() {
	pthread_rwlock_unlock(&usersHash_lock);
}

void usersHash_beginWrite() {
	if (pthread_rwlock_wrlock(&usersHash_lock) != 0) syslog(LOG_INFO,"error al begin write lock\n");
}

void usersHash_endWrite() {
	pthread_rwlock_unlock(&usersHash_lock);
}

void userSocket_beginWrite(User *user) {
	if (user) {
		pthread_mutex_lock(&(user->socketLock));
	}
	
}

void userSocket_endWrite(User *user) {
	if (user) {
		pthread_mutex_unlock(&(user->socketLock));
	}
}
