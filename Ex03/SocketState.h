#include "UtilsFunc.h"

struct SocketState
{
	SOCKET id;			// Socket handle
	int	recv;			// Receiving?
	int	send;			// Sending?
	double lastByteRecvTime;
	list<requestType> subTypes = {}; // SUBTYPE LIST (POST,PUT,DELETE, ... etc)
	list<string> reqs = {};
	int sendSubType;	// maybe delete
	char buffer[128]; // maybe delete
	int len; // maybe delete
};