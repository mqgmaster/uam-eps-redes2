#include "../includes/channelsHash.h"

Channel *channelsHash = NULL;     

Channel* channelsHash_put(char *name) {       
	Channel *channel;
	HASH_FIND_STR(channelsHash, name, channel);
	if (channel) {
		return channel;                    
	}
	channel = malloc(sizeof(Channel));
	channel->name = malloc(strlen(name) + 1);
	channel->users = NULL;
	strcpy(channel->name, name);
	HASH_ADD_STR(channelsHash, name, channel);
	return channel;
}

int channelsHash_addUser(Channel *channelTarget, User *user) {
	Channel *channel;
	HASH_FIND_STR(channelsHash, channelTarget->name, channel);
	if (channel) {
		return usersHash_putPointer_(&(channel->users), user);
	} else {
		return FALSE;
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

void channelsHash_printLog() {       
	Channel *channel;
	for (channel = channelsHash; channel != NULL; channel = channel->hh.next) {
		syslog(LOG_INFO,"usuarios del canal (%s)\n", channel->name);
		usersHash_printLog_(&(channel->users));
	}
}
