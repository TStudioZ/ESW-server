#ifndef EPOLLFD_H
#define EPOLLFD_H

#include "EpollInstance.h"

class EpollFd
{
public:
    EpollFd(int fd, EpollInstance &e);
    virtual ~EpollFd();
    void registerFd(uint32_t events);
    void unregisterFd();
    void updateFd(uint32_t events);
    int getFd() const { return fd; }
    virtual void handleEvent(uint32_t events) = 0;
    void printError();
protected:
    int fd;
    EpollInstance &epollInstance;

    friend class EpollInstance;
};

#endif // EPOLLFD_H
