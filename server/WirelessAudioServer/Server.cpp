///////////////////// Includes ////////////////////////////
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include <winsock2.h>
#include <stdio.h>
#include <QDebug>
#include "Server.h"

int ServerSetup(bool tcp) {

    //OPENFILENAME ofnsv;
    //char filenameserver[FILENAMESIZE];
	int ret;
	WSADATA wsaData;
	SOCKADDR_IN InternetAddr;

    /*ZeroMemory(&ofnsv, sizeof(ofnsv));
	ofnsv.lStructSize = sizeof(ofnsv);
	ofnsv.hwndOwner = NULL;
    ofnsv.lpstrFile = (LPWSTR) filenameserver;
	ofnsv.lpstrFile[0] = '\0';
	ofnsv.nMaxFile = sizeof(filenameserver);
	ofnsv.lpstrFilter = TEXT("Text files (.txt)\0*.TXT\0\0");
	ofnsv.nFilterIndex = 1;
	ofnsv.lpstrFileTitle = NULL;
	ofnsv.nMaxFileTitle = 0;
	ofnsv.lpstrInitialDir = NULL;
	ofnsv.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_NOLONGNAMES | OFN_OVERWRITEPROMPT;
	if (GetSaveFileName(&ofnsv) == 0) {
		return 0;
	}

    hServ = CreateFile(filenameserver, // file to open
		GENERIC_WRITE,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		CREATE_ALWAYS,         // existing file only
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL);                 // no attr. template

	hLog = CreateFile("log.txt", // file to open
		GENERIC_WRITE,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		CREATE_ALWAYS,         // existing file only
		FILE_ATTRIBUTE_NORMAL, // normal file
        NULL);                 // no attr. template*/

	if ((ret = WSAStartup(0x0202, &wsaData)) != 0)
	{
		sprintf_s(errmsg, ERRORSIZE, "WSAStartup failed with error %d\n", ret);
        qDebug() << errmsg;
		WSACleanup();
		return -1;
	}

	if (tcp) {
		if ((listensock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0,
			WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
			sprintf_s(errmsg, "Failed to get a socket %d\n", WSAGetLastError());
            qDebug() << errmsg;
			return -1;
		}
	}
	else {
		if ((listensock = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0,
			WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
			sprintf_s(errmsg, "Failed to get a socket %d\n", WSAGetLastError());
            qDebug() << errmsg;
			return -1;
		}
	}

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(SERVER_DEFAULT_PORT);

	if (bind(listensock, (PSOCKADDR)&InternetAddr,
		sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		sprintf_s(errmsg, "bind() failed with error %d\n", WSAGetLastError());
        qDebug() << errmsg;
		return -1;
	}
	if (tcp) {
		if (listen(listensock, 5))
		{
			sprintf_s(errmsg, "listen() failed with error %d\n", WSAGetLastError());
            qDebug() << errmsg;
			return -1;
		}
	}

	if ((AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
	{
		sprintf_s(errmsg, "WSACreateEvent() failed with error %d\n", WSAGetLastError());
        qDebug() << errmsg;
		return -1;
	}
	return 0;
}

DWORD WINAPI ServerListen(LPVOID lpParameter) {
	HANDLE hThread;
	DWORD ThreadId, tmprcv;
	struct ClientParams *serverparam = (struct ClientParams *) lpParameter;
	char buff[2];
	WSABUF bufftmp;
	bufftmp.buf = buff;
	bufftmp.len = 2;

	if ((hThread = CreateThread(NULL, 0, ServerThread, (LPVOID)serverparam->accept, 0, &ThreadId)) == NULL)
	{
		printf("CreateThread failed with error %d\n", GetLastError());
		return FALSE;
	}

	while (TRUE)
	{
		AcceptSocket = accept(serverparam->sock, NULL, NULL);

		if (WSASetEvent(AcceptEvent) == FALSE)
		{
			sprintf_s(errmsg, "WSASetEvent failed with error %d\n", WSAGetLastError());
            qDebug() << errmsg;
			return FALSE;
		}
	}
	return TRUE;
}

DWORD WINAPI ServerThread(LPVOID lpParameter) {
	DWORD Flags;
	WSAEVENT EventArray[1];
	DWORD Index;
	DWORD RecvBytes;
	LPSOCKET_INFORMATION SocketInfo;
	struct sockaddr_in ClientAddr;
    int clientaddrsize = sizeof(ClientAddr);

	// Save the accept event in the event array.

	EventArray[0] = (WSAEVENT)lpParameter;

	while (TRUE)
	{
		// Wait for accept() to signal an event and also process WorkerRoutine() returns.

		while (TRUE)
		{
			Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, TRUE);

			if (Index == WSA_WAIT_FAILED)
			{
				printf("WSAWaitForMultipleEvents failed with error %d\n", WSAGetLastError());
				return FALSE;
			}

			if (Index != WAIT_IO_COMPLETION)
			{
				// An accept() call event is ready - break the wait loop
				break;
			}
		}

		WSAResetEvent(EventArray[Index - WSA_WAIT_EVENT_0]);

		// Create a socket information structure to associate with the accepted socket.

		if ((SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR,
			sizeof(SOCKET_INFORMATION))) == NULL)
		{
			sprintf_s(errmsg, "GlobalAlloc() failed with error %d\n", GetLastError());
            qDebug() << errmsg;
			return FALSE;
		}

		// Fill in the details of our accepted socket.

		SocketInfo->Socket = AcceptSocket;
		ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
		SocketInfo->BytesSEND = 0;
		SocketInfo->BytesRECV = 0;
		SocketInfo->DataBuf.len = KBYTES540;
		SocketInfo->DataBuf.buf = SocketInfo->Buffer;

		sprintf_s(errmsg, "Socket %d connected\n", AcceptSocket);
        qDebug() << errmsg;

		
		Flags = 0;
		if (tcp) {
			if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
				&(SocketInfo->Overlapped), ServerCallback) == SOCKET_ERROR)
			{
				ShowLastErr(true);
				if (WSAGetLastError() != WSA_IO_PENDING)
				{
					sprintf_s(errmsg, "WSARecv() failed with error %d\n", WSAGetLastError());
                    qDebug() << errmsg;
					return FALSE;
				}
			}
		}
		else {
			if (WSARecvFrom(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
				(SOCKADDR *)&ClientAddr, &clientaddrsize, &(SocketInfo->Overlapped), ServerCallback) == SOCKET_ERROR)
			{
				ShowLastErr(true);
				if (WSAGetLastError() != WSA_IO_PENDING)
				{
					sprintf_s(errmsg, "WSARecv() failed with error %d\n", WSAGetLastError());
                    qDebug() << errmsg;
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

void CALLBACK ServerCallback(DWORD Error, DWORD BytesTransferred,
	LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
	DWORD RecvBytes;
	DWORD Flags;
	DWORD byteswrittenfile = 0;
	// Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
	SI = (LPSOCKET_INFORMATION)Overlapped;
	char *writebuff = SI->Buffer;

	if (Error != 0)
	{
		sprintf_s(errmsg, "I/O operation failed with error %d\n", Error);
        qDebug() << errmsg;
	}

	if (BytesTransferred == 0)
	{
		sprintf_s(errmsg, "Closing socket %d\n", SI->Socket);
        qDebug() << errmsg;
	}

	if (Error != 0 || BytesTransferred == 0)
	{
		closesocket(SI->Socket);
		GlobalFree(SI);
		return;
	}

	sprintf_s(errmsg, "Bytes received: %d\n", BytesTransferred);
    qDebug() << errmsg;

	if (WriteFile(hServ, writebuff, BytesTransferred, &byteswrittenfile, NULL) == FALSE) {
        qDebug() << "Couldn't write to server file\n";
		ShowLastErr(false);
	}

	Flags = 0;
	ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

	SI->DataBuf.len = KBYTES540;
	SI->DataBuf.buf = SI->Buffer;

	if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags,
		&(SI->Overlapped), ServerCallback) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			sprintf_s(errmsg, "WSARecv() failed with error %d\n", WSAGetLastError());
            qDebug() << errmsg;
			return;
		}
	}
}

void ServerCleanup() {
	CloseHandle(hServ);
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
