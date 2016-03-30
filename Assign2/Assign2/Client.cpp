///////////////////// Includes ////////////////////////////
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include <winsock2.h>
#include <stdio.h>
#include "Globals.h"

#pragma comment(lib, "Ws2_32.lib")

///////////////////// Prototypes //////////////////////////
void ClientCleanup(SOCKET s);

//////////////////// Globals //////////////////////////////
HANDLE hSendFile;
struct sockaddr_in server, client;
SOCKET sClient;

int ClientSetup(bool tcp) {
	WSADATA WSAData;
	WORD wVersionRequested;
	struct hostent	*hp;

	wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &WSAData) != 0) {
		OutputDebugString("DLL not found!\n");
		ShowLastErr(true);
		return -1;
	}

	if (tcp) {
		if ((sClient = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			OutputDebugString("Cannot create tcp socket\n");
			return -1;
		}
	}
	else {
		if ((sClient = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		{
			OutputDebugString("Cannot create udp socket\n");
			return -1;
		}
	}

	// Initialize and set up the address structure
	memset((char *)&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_DEFAULT_PORT);
	if ((hp = gethostbyname(address)) == NULL)
	{
		OutputDebugString("Unknown server address\n");
		return -1;
	}

	// Copy the server address
	memcpy((char *)&server.sin_addr, hp->h_addr, hp->h_length);

	if (tcp) {
		// Connecting to the server
		if (connect(sClient, (struct sockaddr *)&server, sizeof(server)) == -1)
		{
			OutputDebugString("Can't connect to server\n");
			ShowLastErr(true);
			return -1;
		}
	} else {
		// Bind local address to the socket
		memset((char *)&client, 0, sizeof(client));
		client.sin_family = AF_INET;
		client.sin_port = htons(0);  // bind to any available port
		client.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(sClient, (struct sockaddr *)&client, sizeof(client)) == -1)
		{
			perror("Can't bind name to socket");
			return -1;
		}
	}

	return 0;
}

DWORD WINAPI ClientSend(LPVOID strucParams) {
	struct ClientParams *clientparam = (struct ClientParams *) strucParams;
	char *sendbuff = new char[clientparam->size];
	DWORD  dwBytesRead;
	int sentBytes;
	int sentpackets = 0;
	struct sockaddr_in sockadd = clientparam->server;

	clientrunning = true;

	hSendFile = CreateFile(clientparam->filename, // file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL);                 // no attr. template

	while (true) {
		if (ReadFile(hSendFile, sendbuff, clientparam->size - 1, &dwBytesRead, NULL) == FALSE)
		{
			if (sentpackets < clientparam->numpackets && clientparam->numpackets != 1) {
				SetFilePointer(hSendFile, 0, NULL, FILE_BEGIN);
			}
			else if (dwBytesRead == 0) {
				ClientCleanup(clientparam->sock);
				return TRUE;
			}
			else {
				OutputDebugString("Couldn't read instructions\n");
				ClientCleanup(clientparam->sock);
				return FALSE;
			}
		}

		if (dwBytesRead > 0 && dwBytesRead < (DWORD)clientparam->size - 1)
		{
			sendbuff[dwBytesRead] = '\0';
		}

		if (clientparam->tcp) {
			sentBytes = send(clientparam->sock, sendbuff, clientparam->size, 0);
			ShowLastErr(true);
			sentpackets++;
		}
		else {
			sentBytes = sendto(clientparam->sock, sendbuff, clientparam->size, 0, (struct sockaddr *)&sockadd, sizeof(sockadd));
			ShowLastErr(true);
			sentpackets++;
		}
		if (sentpackets > clientparam->numpackets && clientparam->numpackets != 1) {
			ClientCleanup(clientparam->sock);
			return TRUE;
		}
	}
	
	return TRUE;
}

void ClientCleanup(SOCKET s) {
	closesocket(s);
	CloseHandle(hSendFile);
	WSACleanup();
	clientrunning = false;
}