#ifndef SERVER_H
#define SERVER_H

#include <Windows.h>

///////////////////// Macros //////////////////////////////
#define SERVER_DEFAULT_PORT	7000
#define FILENAMESIZE		100
#define ERRORSIZE			512
#define KBYTES540		4423680

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
    CHAR Buffer[KBYTES540];
    WSABUF DataBuf;
    DWORD BytesSEND;
    DWORD BytesRECV;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

///////////////////// Global Variables ////////////////////
char address[100];
SOCKET sClient, listensock, AcceptSocket;
struct sockaddr_in server;
WSAEVENT AcceptEvent;
HANDLE hSendFile, hServ, hLog;
LPSOCKET_INFORMATION SI;
bool tcp;
char errmsg[ERRORSIZE];

///////////////////// Global Prototypes ///////////////////
void ShowLastErr(bool wsa);
int ServerSetup(bool tcp);
DWORD WINAPI ServerListen(LPVOID lpParameter);
void ServerCleanup();
DWORD WINAPI ServerThread(LPVOID lpParameter);
void CALLBACK ServerCallback(DWORD Error, DWORD BytesTransferred,
    LPWSAOVERLAPPED Overlapped, DWORD InFlags);

#endif
