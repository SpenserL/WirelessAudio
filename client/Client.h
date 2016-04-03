#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <windows.h>

///////////////////// Global Prototypes ///////////////////
void ShowLastErr(bool wsa);
int ClientSetup(char* addr);
void ClientCleanup();
int ClientSend(HANDLE hFile);
DWORD WINAPI ClientSendThread(LPVOID lpParameter);
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
extern char address[100];
extern SOCKET sendSock;
extern struct sockaddr_in server;
extern char errMsg[ERRORSIZE];

#endif
