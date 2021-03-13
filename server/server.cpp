#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <WinSock2.h>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int addrlenServer, addrlenClient, BytesReceived, userCount = 0;
SOCKET ListeningSocket, NewConnection, Clients[10];
SOCKADDR_IN serverAddr, senderInfo;

void send_to_all_of_them(SOCKET sock, int a);

int main() {
	int port = 7171;
	int i = 0;
	for (i = 0; i < 10; i++) {
		Clients[i] = 0;
	}

	//Initialize WSA
	WSADATA wsadata;
	WORD version = MAKEWORD(2, 2);
	int result = WSAStartup(version, &wsadata);
	if (result != 0) {
		cout << "WSA Initialization is successfull" << endl;
	}
	else {
		cout << "The winsock DLL found.\nThe current status is: " << wsadata.szSystemStatus << endl;
	}
	//Initialize WSA

	BOOL optval = TRUE;
	int optlen = sizeof(BOOL);

	//Create the first socket listen for connections
	ListeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListeningSocket == INVALID_SOCKET) {
		cout << "Server: Error at socket(), error code: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}
	else {
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		addrlenServer = sizeof(serverAddr);
		result = bind(ListeningSocket, (SOCKADDR*)&serverAddr, addrlenServer);
		if (result == SOCKET_ERROR) {
			cout << "Server: bind() failed! Error code: " << WSAGetLastError() << endl;
			closesocket(ListeningSocket);
			WSACleanup();
			return 1;
		}
		else {
			cout << "Server: bind() is OK!\n*****************************" << endl;
			getsockname(ListeningSocket, (SOCKADDR*)&serverAddr, &addrlenServer);
			cout << "Main Socket: " << ListeningSocket << "\nIP: " << inet_ntoa(serverAddr.sin_addr) << "\nPort: " << htons(serverAddr.sin_port) << "\n*****************************" << endl;
			int a = 0;
		}

		result = setsockopt(ListeningSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, optlen);
		if (result == SOCKET_ERROR) {
			cout << "Server: noDelay failed! Error code: " << WSAGetLastError() << endl;
			closesocket(ListeningSocket);
			WSACleanup();
			return 1;
		}
	}

	result = listen(ListeningSocket, 5);
	if (result == SOCKET_ERROR) {
		cout << "Server: listen(): Error listening on socket " << WSAGetLastError() << endl;
		closesocket(ListeningSocket);
		WSACleanup();
		return 1;
	}
	else cout << "Server: listen() is OK, I'm listening for connections..." << endl;
	thread client_threads[10];
	while (1) {
		NewConnection = accept(ListeningSocket, (sockaddr*)&serverAddr, &addrlenServer);
		if (NewConnection == SOCKET_ERROR) {
			cout << "Error at accepting a new client. Error code: " << WSAGetLastError() << endl;
			WSACleanup();
		}
		else {
			cout << "***************************\nClient accepted." << endl;
			memset(&senderInfo, 0, sizeof(senderInfo));
			addrlenClient = sizeof(senderInfo);
			getpeername(NewConnection, (sockaddr*)&senderInfo, &addrlenClient);
			cout << "Client IP: " << inet_ntoa(senderInfo.sin_addr) << "\nPort: " << htons(senderInfo.sin_port) << endl;
			userCount++;
		}

		for (i = 0; i < 10; i++) {
			if (Clients[i] == 0) {
				Clients[i] = NewConnection;
				result = setsockopt(Clients[i], IPPROTO_TCP, TCP_NODELAY, (char*)&optval, optlen);
				if (result == SOCKET_ERROR) {
					cout << "Server: noDelay failed! Error code: " << WSAGetLastError() << endl;
					closesocket(ListeningSocket);
					WSACleanup();
					return 1;
				}
				cout << "New client added to the client list. Client number: " << i << "\nSOCKET VALUE: " << Clients[i] << "\n*********************************" << endl;
				client_threads[i] = thread(send_to_all_of_them, Clients[i], i);
				break;
			}
		}
	}

	return 0;
}

void send_to_all_of_them(SOCKET sock, int a) {
	cout << "thread started" << endl;
	int i = 0;

	int receive;
	while (1) {
		char* buffer = new char[4096];
		receive = recv(sock, buffer, 4096, 0);
		cout << buffer << endl;
		for (i = 0; i < 10; i++)
		{
			if (i != a) send(Clients[i], buffer, strlen(buffer) + 1, 0);
		}
		delete[] buffer;
	}
	return;
}