#ifndef EPOLLINSTANCE_H
#define EPOLLINSTANCE_H

#include <mutex>

#define EPOLL_MAX_EVENTS 64

class EpollFd;

class EpollInstance
{
private:
    std::mutex _mutex;
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
