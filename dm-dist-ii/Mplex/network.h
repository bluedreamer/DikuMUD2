#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define DEF_SERVER_ADDR "127.0.0.1"

int OpenMother(int port);
int OpenNetwork(int nMudPort, const char *pMudAddr);
