#pragma once
#include <winsock2.h>
#include <iostream>
#include "SocketState.h"
using namespace std;

class ServerManger
{
private:
	SocketState sockets[MAX_SOCKETS] = { 0 };
	int socketsCount = 0;

public: // maybe change params from index to array
	static SOCKET initListenSocket();
	static void bindSocket(SOCKET listenSocket);
	bool addSocket(SOCKET id, int what);
	void removeSocket(int indexInArr);
	void checkPassTime();
	void setWaitRecv(fd_set& waitRecv);
	void setWaitSend(fd_set& waitSend);
	void handleRecv(fd_set& waitRecv, int& nfd);
	void handleSend(fd_set& waitSend, int& nfd);
	void acceptConnection(int indexInArr);
	void receiveMessage(int indexInArr);
	void sendMessage(int indexInArr);
};
