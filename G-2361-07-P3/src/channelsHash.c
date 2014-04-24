#include "../includes/channelsHash.h"

Channel *channelsHash = NULL;  
pthread_rwlock_t channelsHash_lock;

void channelsHash_init() {
	if (pthread_rwlock_init(&channelsHash_lock,NULL) != 0) syslog(LOG_INFO,"error al crear lock\n");
}

Channel* channelsHash_getAll() {
	return channelsHash;
}   

void channelsHash_put(char *name, char * topic) {   
	Channel *channel;
	HASH_FIND_STR(channelsHash, name, channel);
	if (channel) {
		return;                    
	}
	channel = malloc(sizeof(Channel));
	channel->name = malloc(strlen(name) + 1);
	channel->topic = malloc(strlen(topic) + 1);
	channel->users = NULL;
	strcpy(channel->topic, "topic");
	strcpy(channel->name, name);
	HASH_ADD_STR(channelsHash, name, channel);
}

void channelsHash_addUser(Channel *channel, User *user) {
	if (channel != NULL && user != NULL) {
		usersHash_put_(&(channel->users), user->socketId, user->nick);
	}
}

int channelsHash_existsUser(Channel *channel, User *user) {
	User *userTemp;
	if (channel != NULL && user != NULL) {
		for (userTemp = channel->users; userTemp != NULL; userTemp = userTemp->hh.next) {
			if(user->socketId == userTemp->socketId) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

Channel* channelsHash_get(char *name) {
	Channel *channel;
	HASH_FIND_STR(channelsHash, name, channel);
	return channel;
}

void channelsHash_delete(char *name) {
	Channel *channel;
	HASH_FIND_STR(channelsHash, name, channel);
	if (channel) {
		syslog(LOG_INFO,"canal (%s) removido\n", channel->name);
		HASH_DEL(channelsHash, channel);                 
	} 
}

void channelsHash_deleteUser(Channel *channel, User *user) {
	if (channel != NULL && user != NULL) {
		syslog(LOG_INFO,"usuario (%s) va a ser removido del canal (%s)\n", user->nick, channel->name);
		usersHash_delete_(&(channel->users), user->socketId);        
	} 
}

void channelsHash_deleteUserFromAll(User *user) {
	if (user) {
		User *channelUser, *tmpUser;
		Channel *channel, *tmpChannel;
		HASH_ITER(hh, channelsHash, channel, tmpChannel) {
			HASH_ITER(hh, channel->users, channelUser, tmpUser) {
				if (channelUser->socketId == user->socketId) {
					channelsHash_deleteUser(channel, channelUser);
				}
			}
		}
	}
}

void channelsHash_printLog() {
	Channel *channel;
	for (channel = channelsHash; channel != NULL; channel = channel->hh.next) {
		syslog(LOG_INFO,"usuarios del canal (%s)(%s)\n", channel->name,channel->topic);
		usersHash_printLog_(&(channel->users));
	}
}

int channelsHash_size() {
	int size = HASH_COUNT(channelsHash);
	syslog(LOG_INFO,"total de canales: %d\n", size);
	return size;
}

int channelsHash_usersSize(Channel *channel) {
	if (channel) {
		syslog(LOG_INFO,"total de usuarios del canal (%s):", channel->name);
		int size = usersHash_size_(&(channel->users));
		return size;
	}
}

void channelsHash_beginRead() {
	if (pthread_rwlock_rdlock(&channelsHash_lock) != 0) syslog(LOG_INFO,"error al begin read\n");
}

void channelsHash_endRead() {
	pthread_rwlock_unlock(&channelsHash_lock);
}

void channelsHash_beginWrite() {
	if (pthread_rwlock_wrlock(&channelsHash_lock) != 0) syslog(LOG_INFO,"error al begin write\n");
}

void channelsHash_endWrite() {
	pthread_rwlock_unlock(&channelsHash_lock);
}
