#include "epollfd.h"

EpollFd::EpollFd(int fd, EpollInstance &e)
    : fd(fd), epollInstance(e)
{
}

EpollFd::~EpollFd()
{
}

void EpollFd::registerFd(uint32_t events)
{
     epollInstance.registerFd(*this, events);
}

void EpollFd::unregisterFd()
{
     epollInstance.unregisterFd(*this);
}

void EpollFd::updateFd(uint32_t events)
{
     epollInstance.updateFd(*this, events);
}
