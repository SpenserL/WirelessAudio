///////////////////// Includes ////////////////////////////
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

//#include <Ws2tcpip.h>
#include <stdio.h>
#include <QDebug>
#include "Client.h"

//#pragma comment(lib, "Ws2_32.lib")

////////// "Real" of the externs in Client.h ///////////////
char address[100];
SOCKET sendSock;
struct sockaddr_in server;
char errMsg[ERRORSIZE];

/////////////////// Globals ////////////////////////////////
HANDLE hSendFile;

int ClientSetup(char* addr) {
	WSADATA WSAData;
	WORD wVersionRequested;
	struct hostent	*hp;
    strcpy(address, addr);

	wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &WSAData) != 0) {
        ShowLastErr(true);
        qDebug() << "DLL not found!\n";
		return -1;
	}

    // TCP Open Socket
    if ((sendSock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        ShowLastErr(true);
        qDebug() << "Cannot create tcp socket\n";
        return -1;
    }

    // UDP Open Socket (if needed in future) ////////////////////
    /*if ((sendSock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
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
        ShowLastErr(true);
        qDebug() << "Unknown server address\n";
		return -1;
	}

	// Copy the server address
	memcpy((char *)&server.sin_addr, hp->h_addr, hp->h_length);

	
    // TCP Connecting to the server
    if (connect(sendSock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        ShowLastErr(true);
        qDebug() << "Can't connect to server\n";
        return -1;
    }

    // UDP Connecting to the server (if needed in future) /////////////
    // Bind local address to the socket
    /*memset((char *)&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(0);  // bind to any available port
    client.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sendSock, (struct sockaddr *)&client, sizeof(client)) == -1)
    {
        perror("Can't bind name to socket");
        return -1;
    }*/

    qDebug() << "Setup success";
	return 0;
}

int ClientSend(HANDLE hFile) {
    HANDLE hThread;
    DWORD ThreadId;

    if ((hThread = CreateThread(NULL, 0, ClientSendThread, (LPVOID)hFile, 0, &ThreadId)) == NULL)
    {
        ShowLastErr(false);
        qDebug() << "Create Send Thread failed";
        return -1;
    }
    return 0;
}

DWORD WINAPI ClientSendThread(LPVOID lpParameter) {
    hSendFile = (HANDLE) lpParameter;
    char *sendbuff = new char[CLIENT_PACKET_SIZE];
	DWORD  dwBytesRead;
    int sentBytes = 0, sentpackets = 0;

	while (true) {
        if (ReadFile(hSendFile, sendbuff, CLIENT_PACKET_SIZE - 1, &dwBytesRead, NULL) == FALSE)
		{
            ShowLastErr(false);
            qDebug() << "Couldn't read file";
            ClientCleanup();
            return FALSE;
		}

        if (dwBytesRead == 0) {
            qDebug() << "End of file";
            ClientCleanup();
            return TRUE;
        }
        else if (dwBytesRead > 0 && dwBytesRead < (DWORD)CLIENT_PACKET_SIZE - 1)
		{
			sendbuff[dwBytesRead] = '\0';
		}

        // TCP Send
        sentBytes = send(sendSock, sendbuff, CLIENT_PACKET_SIZE, 0);
        ShowLastErr(true);
        sentpackets++;
        qDebug() << "Packet" << sentpackets << "sent";

        // UDP send (if needed in future) //////////////////////
        //sentBytes = sendto(clientparam->sock, sendbuff, clientparam->size, 0, (struct sockaddr *)&sockadd, sizeof(sockadd));
        //ShowLastErr(true);
        //sentpackets++;
	}
	
	return TRUE;
}

void ClientCleanup() {
    closesocket(sendSock);
    if (hSendFile) {
        qDebug() << "File Handle Closed";
        CloseHandle(hSendFile);
    }
    qDebug() << "WSACleanup called";
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
    LPCTSTR errMsg = NULL;
    char errnum[100];
    if (wsa == true) {
        dlasterr = WSAGetLastError();
    }
    else {
        dlasterr = GetLastError();
    }
    sprintf_s(errnum, "Error number: %d\n", dlasterr);
    qDebug() << errnum;
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL, dlasterr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&errMsg, 0, NULL);
    qDebug() << QString::fromWCharArray(errMsg);
}
