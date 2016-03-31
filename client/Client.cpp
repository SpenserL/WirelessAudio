///////////////////// Includes ////////////////////////////
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

//#include <Ws2tcpip.h>
#include <stdio.h>
#include <QDebug>
#include "Client.h"

#pragma comment(lib, "Ws2_32.lib")

//////////////////// Globals //////////////////////////////
HANDLE hSendFile;
struct sockaddr_in client;

////////// "Real" of the externs in Client.h ///////////////
SOCKET clientSock;
char address[100];
SOCKET sClient, listensock, AcceptSocket;
struct sockaddr_in server;
WSAEVENT AcceptEvent;
LPSOCKET_INFORMATION SI;
char errmsg[ERRORSIZE];

int ClientSetup() {
	WSADATA WSAData;
	WORD wVersionRequested;
	struct hostent	*hp;

	wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &WSAData) != 0) {
        qDebug() << "DLL not found!\n";
		ShowLastErr(true);
		return -1;
	}

    // TCP Open Socket
    if ((sClient = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        qDebug() << "Cannot create tcp socket\n";
        return -1;
    }

    // UDP Open Socket (if needed in future) ////////////////////
    /*if ((sClient = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        qDebug() << "Cannot create udp socket\n";
        return -1;
    }*/

	// Initialize and set up the address structure
	memset((char *)&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_DEFAULT_PORT);
	if ((hp = gethostbyname(address)) == NULL)
	{
        qDebug() << "Unknown server address\n";
		return -1;
	}

	// Copy the server address
	memcpy((char *)&server.sin_addr, hp->h_addr, hp->h_length);

	
    // TCP Connecting to the server
    if (connect(sClient, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        qDebug() << "Can't connect to server\n";
        ShowLastErr(true);
        return -1;
    }

    // UDP Connecting to the server (if needed in future) /////////////
    // Bind local address to the socket
    /*memset((char *)&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(0);  // bind to any available port
    client.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sClient, (struct sockaddr *)&client, sizeof(client)) == -1)
    {
        perror("Can't bind name to socket");
        return -1;
    }*/

    qDebug() << "Setup success";
	return 0;
}

DWORD WINAPI ClientSend() {
    char *sendbuff = new char[CLIENT_PACKET_SIZE];
	DWORD  dwBytesRead;
	int sentBytes;
    int sentpackets = 0;

    /*hSendFile = CreateFile(clientparam->filename, // file to open
        GENERIC_READ,          // open for reading
        FILE_SHARE_READ,       // share for reading
        NULL,                  // default security
        OPEN_EXISTING,         // existing file only
        FILE_ATTRIBUTE_NORMAL, // normal file
        NULL);                 // no attr. template*/

	while (true) {
        if (ReadFile(hSendFile, sendbuff, CLIENT_PACKET_SIZE - 1, &dwBytesRead, NULL) == FALSE)
		{
            if (dwBytesRead == 0) {
                qDebug() << "End of file";
                ClientCleanup(clientSock);
				return TRUE;
			}
            else {
                qDebug() << "Couldn't read file\n";
                ClientCleanup(clientSock);
				return FALSE;
			}
		}

        if (dwBytesRead > 0 && dwBytesRead < (DWORD)CLIENT_PACKET_SIZE - 1)
		{
			sendbuff[dwBytesRead] = '\0';
		}

        // TCP Send
        sentBytes = send(clientSock, sendbuff, CLIENT_PACKET_SIZE, 0);
        ShowLastErr(true);
        sentpackets++;

        // UDP send (if needed in future) //////////////////////
        //sentBytes = sendto(clientparam->sock, sendbuff, clientparam->size, 0, (struct sockaddr *)&sockadd, sizeof(sockadd));
        //ShowLastErr(true);
        //sentpackets++;
	}
	
	return TRUE;
}

void ClientCleanup(SOCKET s) {
	closesocket(s);
	CloseHandle(hSendFile);
    WSACleanup();
}

/*---------------------------------------------------------------------------------------
--	FUNCTION:		void ShowLastErr()

--
--	DATE:			January 19, 2016
--
--	REVISIONS:		February 13, 2016
--
--	DESIGNERS:		Micah Willems
--
--	PROGRAMMER:		Micah Willems
--
--	NOTES:
--      This is a universal-purpose error reporting function for functions that can be
--		error checked using GetLastError. It uses FormatMessage to get an actual
--		human-readable, understandable string from the error ID and outputs it to the
--		Debug output console in the IDE.
---------------------------------------------------------------------------------------*/
void ShowLastErr(bool wsa) {
    DWORD dlasterr;
    LPCTSTR errmsg = NULL;
    char errnum[100];
    if (wsa = true) {
        dlasterr = WSAGetLastError();
    }
    else {
        dlasterr = GetLastError();
    }
    sprintf_s(errnum, "Error number: %d\n", dlasterr);
    qDebug() << errnum;
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL, dlasterr, 0, (LPWSTR)&errmsg, 0, NULL);
    qDebug() << errmsg;
}
