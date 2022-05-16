#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "ServerManger.h"
#include "UtilsFunc.h"
#include "Request.h" // DELETE later

SOCKET ServerManger::initListenSocket() {

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == listenSocket)
	{
		WSACleanup();
		throw exception("Web Server: Error at socket(): " + WSAGetLastError());
	}

	return listenSocket;
}

void ServerManger::bindSocket(SOCKET listenSocket) {

	sockaddr_in serverService;
	serverService.sin_family = AF_INET;
	serverService.sin_addr.s_addr = INADDR_ANY;
	serverService.sin_port = htons(PORT);

	if (SOCKET_ERROR == bind(listenSocket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		closesocket(listenSocket);
		WSACleanup();
		throw exception("Time Server: Error at bind(): " + WSAGetLastError());
	}
}

bool ServerManger::addSocket(SOCKET id, int what) {

	for (int i = 0; i < MAX_SOCKETS; i++)
	{
		if (sockets[i].recv == EMPTY)
		{
			unsigned long flag = 1;
			if (ioctlsocket(id, FIONBIO, &flag) != 0)  // If flag = 1 -> NONBLOCKING else -> BLOCKING .  
			{
				cout << "Web Server: Error at ioctlsocket(): " << WSAGetLastError() << endl;
			}
			sockets[i].id = id;
			sockets[i].recv = what;
			sockets[i].send = IDLE;
			sockets[i].len = 0;
			sockets[i].lastByteRecvTime = GetTickCount();
			socketsCount++;
			return (true);
		}
	}
	return (false);
}

void ServerManger::checkPassTime() {

	double getCurrTime = GetTickCount();

	for (int i = 1; i < MAX_SOCKETS; i++) {

		if (sockets[i].recv != EMPTY && (getCurrTime - sockets[i].lastByteRecvTime) > TWO_MIN) {
			cout << "\n\n\n\n killed the socket!! \n\n\n\n\n\n\n" << endl;
			closesocket(sockets[i].id);
			removeSocket(i);
		}
	}
}

void ServerManger::removeSocket(int indexInArr) {

	sockets[indexInArr].recv = EMPTY;
	sockets[indexInArr].send = EMPTY;
	socketsCount--;
}

void ServerManger::setWaitRecv(fd_set& waitRecv) {

	FD_ZERO(&waitRecv);
	for (int i = 0; i < MAX_SOCKETS; i++)
	{
		if ((sockets[i].recv == LISTEN) || (sockets[i].recv == RECEIVE))
			FD_SET(sockets[i].id, &waitRecv);
	}
}

void ServerManger::setWaitSend(fd_set& waitSend) {
	
	FD_ZERO(&waitSend);
	for (int i = 0; i < MAX_SOCKETS; i++)
	{
		if (sockets[i].send == SEND)
			FD_SET(sockets[i].id, &waitSend);
	}
}

void ServerManger::handleRecv(fd_set& waitRecv, int& nfd) {

	for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
	{
		if (FD_ISSET(sockets[i].id, &waitRecv)) // Check if socket is in set 
		{
			nfd--; 
			switch (sockets[i].recv)
			{
			case LISTEN: 
				acceptConnection(i);  // Accept new client
				break;

			case RECEIVE:
				receiveMessage(i);  // This socket recieved message
				break;
			}
		}
	}
}

void ServerManger::handleSend(fd_set& waitSend, int& nfd) {

	for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
	{
		if (FD_ISSET(sockets[i].id, &waitSend)) // Check if socket is in set
		{
			nfd--;
			sendMessage(i); 
		}
	}
}

void ServerManger::acceptConnection(int indexInArr) {

	SOCKET id = sockets[indexInArr].id;
	struct sockaddr_in from;
	int fromLen = sizeof(from);

	SOCKET msgSocket = accept(id, (struct sockaddr*)&from, &fromLen); // msgSocket = New client
	if (INVALID_SOCKET == msgSocket)
	{
		cout << "Web Server: Error at accept(): " << WSAGetLastError() << endl;
		return;
	}
	cout << "Web Server: Client " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << " is connected." << endl;

	//unsigned long flag = 1;
	//if (ioctlsocket(msgSocket, FIONBIO, &flag) != 0)  // If flag = 1 -> NONBLOCKING else -> BLOCKING .  
	//{
	//	cout << "Web Server: Error at ioctlsocket(): " << WSAGetLastError() << endl;
	//}

	if (addSocket(msgSocket, RECEIVE) == false) // Add to socket's array (if space available )
	{
		cout << "\t\tToo many connections, dropped!\n";
		closesocket(id);
	}
	return;
}

void ServerManger::receiveMessage(int index) {

	SOCKET msgSocket = sockets[index].id; 

	char tempBuffer[MAX_SIZE];  
	int bytesRecv = recv(msgSocket, tempBuffer, sizeof(tempBuffer), 0); // Maybe need to change to tempbuffer to socket.buffer[]
	tempBuffer[bytesRecv] = '\0';

	sockets[index].lastByteRecvTime = GetTickCount(); // Saves current time

	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Web Server: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(msgSocket);
		removeSocket(index);
		return;
	}

	if (bytesRecv == 0)
	{
		closesocket(msgSocket);
		removeSocket(index);
		return;
	}
	else
	{
		cout << "Web Server: Recieved: " << bytesRecv << " bytes of \n\"" << tempBuffer << "\" message.\n";

		string method = getMethod(tempBuffer); // Get method name from reqs
		sockets[index].reqs.push_back(tempBuffer);  // Push new request to end of list (increases list's size by 1)
		sockets[index].send = SEND; 
		sockets[index].subTypes.push_back(convertStrToEnumType(method));  // Convert string to enum and push it to SUBTYPE list

		return;
	}
}

void ServerManger::sendMessage(int index) {
	int bytesSent = 0;
	char sendBuff[MAX_SIZE] = { 0 };	
	string ans;

	SOCKET msgSocket = sockets[index].id; // Get socket

	switch (sockets[index].subTypes.front())  // return reference of first subType in list
	{
		case requestType::GET_TYPE: 
		{
			ans = handleResponseGet(sockets[index].reqs.front()); //we need to take the first in line ...
			break;
		}
		case requestType::POST_TYPE:
		{
			ans = handleResponsePost(sockets[index].reqs.front());
			break;
		}
		case requestType::PUT_TYPE:
		{
			ans = handleResponsePut(sockets[index].reqs.front());
			break;
		}
		case requestType::DELETE_TYPE:
		{
			ans = handleResponseDelete(sockets[index].reqs.front());
			break;
		}
		case requestType::TRACE_TYPE:
		{
			ans = handleResponseTrace(sockets[index].reqs.front());
			break;
		}
		case requestType::HEAD_TYPE:
		{
			ans = handleResponseHead(sockets[index].reqs.front());
			break;
		}
		case requestType::OPTIONS_TYPE:
		{
			ans = handleResponseOptions(sockets[index].reqs.front());
			break;
		}
	}

	strcpy(sendBuff, ans.c_str());
	bytesSent = send(msgSocket, sendBuff, (int)strlen(sendBuff), 0);
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Web Server: Error at send(): " << WSAGetLastError() << endl;
		return;
	}

	cout << "Web Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" <<
		endl << sendBuff << "\" message.\n";

	sockets[index].reqs.pop_front();
	sockets[index].subTypes.pop_front();

	if (sockets[index].reqs.empty())
		sockets[index].send = IDLE;
}