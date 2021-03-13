#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <WinSock2.h>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

SOCKET clientsocket;
SOCKADDR_IN serverAddr, senderInfo;
int addrlenServer, addrlenClient;

void send_message();
void receive_message();

struct message_size_header {
	short unsigned int string_size;
};

int main() {
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
	clientsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientsocket == INVALID_SOCKET) {
		cout << "Server: Error at socket(), error code: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}
	else {
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(7171);
		serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		addrlenServer = sizeof(serverAddr);
		addrlenClient = sizeof(senderInfo);

		result = setsockopt(clientsocket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, optlen);
		if (result == SOCKET_ERROR) {
			cout << "Server: noDelay failed! Error code: " << WSAGetLastError() << endl;
			closesocket(clientsocket);
			WSACleanup();
			return 1;
		}

		result = connect(clientsocket, (sockaddr*)&serverAddr, addrlenServer);

		if (result == SOCKET_ERROR) {
			cout << "Conenction error. Error at connect(). Error code: " << WSAGetLastError() << endl;
			closesocket(clientsocket);
			WSACleanup();
			return 1;
		}

		thread recv_message(receive_message); //ana thread'ten ayrý bir thread receive_message'yi yönetiyor
		send_message(); //main thread ise send_message'yi yönetiyor.
	}

	closesocket(clientsocket);
	WSACleanup();
	return 0;
}

void receive_message() {
	cout << "recive thread is created" << endl;
	int result;
	while (1) {
		char* buffer = new char[4096];
		result = recv(clientsocket, buffer, 4096, 0);
		cout << "sender: " << buffer << endl;
		delete[] buffer;
	}
}

void send_message() {
	cout << "send thread is created" << endl;
	int i = 0;
	char eot;
	int result;
	while (1) {
		char* buffer = new char[4096];
		eot = getchar();
		while (true) {
			if (eot == '\n') { buffer[i] = '\0'; break; }
			buffer[i++] = eot;
			eot = getchar();
		}
		i = 0;
		result = send(clientsocket, buffer, strlen(buffer) + 1, 0);
		delete[] buffer;
	}
}

