#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <windows.h>
#include <QBuffer>

///////////////////// Global Prototypes ///////////////////
void ShowLastErr(bool wsa);
int ClientSetup(char* addr);
void ClientCleanup();
int ClientSend(HANDLE hFile);
int ClientSendMicrophoneData(HANDLE hFile);
DWORD WINAPI ClientSendMicrophoneThread(LPVOID lpParameter);
DWORD WINAPI ClientSendThread(LPVOID lpParameter);
void ClientCleanup(SOCKET s);

///////////////////// Macros //////////////////////////////
#define SERVER_DEFAULT_PORT 7000
#define FILENAMESIZE        100
#define ERRORSIZE           512
#define CLIENT_PACKET_SIZE  8192

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
extern SOCKET sendSock;
extern struct sockaddr_in server;
extern char errMsg[ERRORSIZE];
extern bool isRecording;
extern QBuffer *microphoneBuffer;

#endif
