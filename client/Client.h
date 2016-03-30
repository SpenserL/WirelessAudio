#ifndef CLIENT_H
#define CLIENT_H

#include <windows.h>

///////////////////// Global Prototypes ///////////////////
void ShowLastErr(bool wsa);
int ClientSetup(bool tcp);
void ClientCleanup(SOCKET s);
DWORD WINAPI ClientSend(LPVOID lpParameter);
void ClientCleanup(SOCKET s);

///////////////////// Macros //////////////////////////////
#define SERVER_DEFAULT_PORT     7000
#define FILENAMESIZE            100
#define ERRORSIZE               512
#define KBYTES540               4423680
#define CLIENT_PACKET_SIZE      4423680

typedef struct _SOCKET_INFORMATION {
    OVERLAPPED Overlapped;
    SOCKET Socket;
    CHAR Buffer[KBYTES540];
    WSABUF DataBuf;
    DWORD BytesSEND;
    DWORD BytesRECV;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

///////////////////// Global Variables ////////////////////
SOCKET clientSock;
char address[100];
SOCKET sClient, listensock, AcceptSocket;
struct sockaddr_in server;
WSAEVENT AcceptEvent;
LPSOCKET_INFORMATION SI;
char errmsg[ERRORSIZE];

#endif
