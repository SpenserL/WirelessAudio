#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <windows.h>

///////////////////// Global Prototypes ///////////////////
void ShowLastErr(bool wsa);
int ClientSetup();
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
extern SOCKET clientSock;
extern char address[100];
extern SOCKET sClient, listensock, AcceptSocket;
extern struct sockaddr_in server;
extern WSAEVENT AcceptEvent;
extern LPSOCKET_INFORMATION SI;
extern char errmsg[ERRORSIZE];

#endif
