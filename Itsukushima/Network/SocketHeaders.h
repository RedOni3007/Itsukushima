#include <Core/CoreHeaders.h>

#ifndef SOCKET_HEADER_H
#define SOCKET_HEADER_H

#if X_PC
//should defined before include winsock2, so put here to make sure
#ifndef FD_SETSIZE
#define FD_SETSIZE 1024
#endif

//windows headers
struct IUnknown;
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#endif

#define UDP_PORT 23456

// get sockaddr, IPv4 or IPv6:
static void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

#endif