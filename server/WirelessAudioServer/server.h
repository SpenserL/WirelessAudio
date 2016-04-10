#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <Windows.h>
#include "../../client/circularbuffer.h"

///////////////////// Macros //////////////////////////////
#define SERVER_DEFAULT_PORT	7001
#define FILENAMESIZE		100
#define ERRORSIZE			512
#define CLIENT_PACKET_SIZE  8192

///////////// Global Structure Definitions ////////////////
struct ClientParams {
    bool tcp;
    char filename[FILENAMESIZE];
    int numpackets;
    long size;
    SOCKET sock;
    struct sockaddr_in server;
    WSAEVENT accept;
};

typedef struct _SOCKET_INFORMATION {
    OVERLAPPED Overlapped;
    SOCKET Socket;
    CHAR Buffer[CLIENT_PACKET_SIZE];
    WSABUF DataBuf;
    DWORD BytesSEND;
    DWORD BytesRECV;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

///////////////////// Global Variables ////////////////////
extern char address[100];
extern SOCKET sClient, listenSock, acceptSock;
extern struct sockaddr_in server;
extern WSAEVENT acceptEvent;
extern HANDLE hSendFile, hServ, hLog;
extern LPSOCKET_INFORMATION SI;
extern char errMsg[ERRORSIZE];
extern CircularBuffer* circularBufferRecv;

///////////////////// Global Prototypes ///////////////////
void ShowLastErr(bool wsa);
int ServerSetup();
int ServerListen(HANDLE hFile);
DWORD WINAPI ServerListenThread(LPVOID lpParameter);
void ServerCleanup();
DWORD WINAPI ServerReceiveThread(LPVOID lpParameter);
void CALLBACK ServerCallback(DWORD Error, DWORD BytesTransferred,
    LPWSAOVERLAPPED Overlapped, DWORD InFlags);
DWORD WINAPI ServerWriteToFileThread(LPVOID lpParameter);

#endif
