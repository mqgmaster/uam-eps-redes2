#include "../includes/channelsHash.h"

Channel *channelsHash = NULL;  

Channel* channelsHash_getAll() {
	return channelsHash;
}   

Channel* channelsHash_put(char *name, char * topic) {       
	Channel *channel;
	HASH_FIND_STR(channelsHash, name, channel);
	if (channel) {
		return channel;                    
	}
	channel = malloc(sizeof(Channel));
	channel->name = malloc(strlen(name) + 1);
	channel->topic = malloc(strlen(topic) + 1);
	channel->users = NULL;
	strcpy(channel->topic, "topic");
	strcpy(channel->name, name);
	HASH_ADD_STR(channelsHash, name, channel);
	return channel;
}

void channelsHash_addUser(Channel *channel, User *user) {
	if (channel) {
		usersHash_put_(&(channel->users), user->socketId, user->nick);
	}
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
	if (channel) {
		syslog(LOG_INFO,"usuario (%s) va a ser removido del canal (%s)\n", user->nick, channel->name);
		usersHash_delete_(&(channel->users), user->socketId);        
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