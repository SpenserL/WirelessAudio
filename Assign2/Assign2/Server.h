#ifndef SERVER_H
#define SERVER_H

int ServerSetup(bool tcp);
DWORD WINAPI ServerListen(LPVOID lpParameter);
void ServerCleanup();

#endif
