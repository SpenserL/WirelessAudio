#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
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
extern char address[100];
extern SOCKET sClient, listensock, AcceptSocket;
extern struct sockaddr_in server;
extern WSAEVENT AcceptEvent;
extern HANDLE hSendFile, hServ, hLog;
extern LPSOCKET_INFORMATION SI;
extern char errmsg[ERRORSIZE];

///////////////////// Global Prototypes ///////////////////
void ShowLastErr(bool wsa);
int ServerSetup();
DWORD WINAPI ServerListen(LPVOID lpParameter);
void ServerCleanup();
DWORD WINAPI ServerThread(LPVOID lpParameter);
void CALLBACK ServerCallback(DWORD Error, DWORD BytesTransferred,
    LPWSAOVERLAPPED Overlapped, DWORD InFlags);

#endif
