#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

#include "EpollTcpServer.h"
#include "TcpConnection.h"

using namespace std;

EpollTcpServer::EpollTcpServer(uint32_t p, EpollInstance &e, MemoryPool & mp, WordsCounter & wc) 
    : EpollFd(-1, e), port(p), memoryPool(mp), wordsCounter(wc)
{
    if (startServer())
    {
        serverStarted = true;
        registerFd(EPOLLIN);
    }
    else
    {
        serverStarted = false;
    }
}

EpollTcpServer::~EpollTcpServer()
{
    //delete[] eps;

    if (serverStarted)
    {
        unregisterFd();
    }
}

void EpollTcpServer::handleEvent(uint32_t events)
{
    if (events & EPOLLERR)
    {
        //cout << "Unregistering server fd..." << endl;
        unregisterFd();
    }
    else if (events & EPOLLIN)
    {
        acceptConnection();
    }
    else
    {
        unregisterFd();
    }
}

bool EpollTcpServer::startServer()
{
    //std::cout << "Starting server on port: " << port << std::endl;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        printError();
        return false;
    }

    struct sockaddr_in saddr;

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family      = AF_INET;              // IPv4
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);    // Bind to all available interfaces
    saddr.sin_port        = htons(port);          // Requested port

    int r = bind(fd, (struct sockaddr *) &saddr, sizeof(saddr));
    if (r == -1)
    {
        printError();
        return false;
    }

    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        printError();
        return false;
    }
    flags |= O_NONBLOCK;
    r = fcntl(fd, F_SETFL, flags);
    if (r == -1)
    {
        printError();
        return false;
    }

    if (listen(fd, SOMAXCONN) == -1)
    {
        printError();
        return false;
    }

    std::cout << "Server listening on port: " << port << std::endl;
    return true;
}

void EpollTcpServer::acceptConnection()
{
    //cout << "Accepting connection...";
    int cfd = accept(fd, NULL, NULL);
    if (cfd == -1)
    {
        printError();
        return;
    }
    new TcpConnection(cfd, epollInstance, memoryPool, wordsCounter);
    //rr = (rr + 1) % EpollTcpServer::HANDLERS_COUNT;
    //std::cout << "Connection established" << std::endl;
}