#ifndef GLOBALS_H
#define GLOBALS_H

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

///////////////////// Global Externs //////////////////////
extern char address[100];
extern SOCKET sClient, listensock, AcceptSocket;
extern struct sockaddr_in server;
extern WSAEVENT AcceptEvent;
extern HANDLE hSendFile, hServ, hLog;
extern bool clientrunning;
extern LPSOCKET_INFORMATION SI;
extern bool tcp;

///////////////////// Global Variables ////////////////////
extern char errmsg[ERRORSIZE];

///////////////////// Global Prototypes ///////////////////
void ShowLastErr(bool wsa);

#endif 
