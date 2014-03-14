#ifndef COMMAND_H
#define COMMAND_H

enum commands {
	CMD_NICK,
	CMD_USER,
	CMD_QUIT,
	CMD_JOIN,
	CMD_PART,
	CMD_PASS,
	CMD_PRIVMSG,
	CMD_AWAY,
	CMD_INFO,
	CMD_INVITE,
	CMD_LIST,
	CMD_NAMES,
	CMD_PING
};

enum response {
	RPL_LIST = 322,
	RPL_LISTEND = 323,
};

#endif