#ifndef CLIENT_H
#define CLIENT_H

int ClientSetup(bool tcp);
void ClientCleanup(SOCKET s);
DWORD WINAPI ClientSend(LPVOID lpParameter);

#endif
