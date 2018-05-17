#include <unistd.h>
#include <sys/epoll.h>
#include <cstring>
#include <stdexcept>
#include "epollinstance.h"
#include "epollfd.h"
#include "epolltcpserver.h"
#include <iostream>
#include <thread>
#include <future>
#include <functional>

using namespace std;

EpollInstance::EpollInstance()
{
    epollFd = epoll_create1(0);
    if (epollFd == -1)
    {
        throw std::runtime_error(std::string("epoll_create1: ") + std::strerror(errno));
    }
}

EpollInstance::~EpollInstance()
{
    close(epollFd);
}

void EpollInstance::registerFd(EpollFd &fd, uint32_t events)
{
    struct epoll_event ev;
    memset(&ev,0,sizeof(ev));

    ev.events = events;
    ev.data.ptr = &fd;

    lock_guard<mutex> lock(_mutex);
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd.fd, &ev) == -1)
    {
        throw std::runtime_error(std::string("Register epoll_ctl: ") + std::strerror(errno));
    }
}

void EpollInstance::unregisterFd(EpollFd &fd)
{
    struct epoll_event ev;
    memset(&ev,0,sizeof(ev));

    ev.events = 0;
    ev.data.ptr = &fd;

    lock_guard<mutex> lock(_mutex);
    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd.fd, &ev) == -1)
    {
        throw std::runtime_error(std::string("Unregister epoll_ctl: ") + std::strerror(errno));
    }
}

void EpollInstance::updateFd(EpollFd & fd, uint32_t events)
{
    struct epoll_event ev;
    memset(&ev,0,sizeof(ev));

    ev.events = events;
    ev.data.ptr = &fd;

    lock_guard<mutex> lock(_mutex);
    if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fd.fd, &ev) == -1)
    {
        throw std::runtime_error(std::string("Update epoll_ctl: ") + std::strerror(errno));
    }
}

void EpollInstance::waitAndHandleEvents()
{
    using namespace std;

    struct epoll_event events[EPOLL_MAX_EVENTS];
    int i, n;
    cout << "Event handler started" << endl;
    while (1)
    { 
        //cout << "Waiting for an event..." << endl;
        n = epoll_wait(epollFd, events, EPOLL_MAX_EVENTS, -1);
        //cout << "Event received" << endl;
        //cout << "Epoll event count: " << n << endl;

        if (n < 0)
        {
            std::cerr << "WaitAndHandleEvents error: " << std::strerror(errno) << std::endl;
            return;
        }

        for (i = 0; i < n; i++)
        {
            EpollFd *fd = static_cast<EpollFd*>(events[i].data.ptr);
            fd->handleEvent(events[i].events);

            // std::packaged_task<void()> t([=] () {
            //     cout << "Processing event..." << endl;
            //     fd->handleEvent(events[i].events);
            // });
            // EpollTcpServer::pool.post(t);
        }
    }
}
