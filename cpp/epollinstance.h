#ifndef EPOLLINSTANCE_H
#define EPOLLINSTANCE_H

#include <stdint.h>

#define EPOLL_MAX_EVENTS 64

class EpollFd;

class EpollInstance
{
public:
    EpollInstance();
    ~EpollInstance();
    void registerFd(EpollFd &fd, uint32_t events);
    void unregisterFd(EpollFd &fd);
    void updateFd(EpollFd & fd, uint32_t events);
    void waitAndHandleEvents();
private:
    int epollFd;
};

#endif // EPOLLINSTANCE_H
