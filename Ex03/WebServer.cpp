#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <string>
#include <fstream>
#include <list>
#include "UtilsFunc.h"
#include "ServerManger.h"

int main() {  

	initializeWinsock();
	SOCKET listenSocket = ServerManger::initListenSocket();
	ServerManger::bindSocket(listenSocket); // Bind listen's socket to port

	if (SOCKET_ERROR == listen(listenSocket, 5)) // Define listen to listen socket (binded to port 8080) 
	{
		closesocket(listenSocket);
		WSACleanup();
		cout << "Web Server: Error at listen(): " << WSAGetLastError() << endl;
	}

	ServerManger serverManger;
	serverManger.addSocket(listenSocket, LISTEN); // Add listen's socket to sockets array (in [0])

	cout << "Web Server: Server is listenning." << endl;

	while (true)
	{
		serverManger.checkPassTime();
		fd_set waitRecv;
		serverManger.setWaitRecv(waitRecv); // Insert all RECIEVE's sockets

		fd_set waitSend;
		serverManger.setWaitSend(waitSend); // Insert all SEND's sockets

		int nfd; 
		nfd = select(0, &waitRecv, &waitSend, NULL, NULL); // Return number of FD's ready to operate
		if (nfd == SOCKET_ERROR) 
		{
			cout << "Web Server: Error at select(): " << WSAGetLastError() << endl;
			WSACleanup();
			return;
		}

		serverManger.handleRecv(waitRecv, nfd); 
		serverManger.handleSend(waitSend, nfd);

	}
	cout << "Web Server: Closing Connection.\n";
	closesocket(listenSocket);
	WSACleanup();
	return 0;
}