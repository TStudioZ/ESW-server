#include <iostream>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <future>
#include "epollinstance.h"
#include "epolltcpserver.h"

using namespace std;

#define EPOLL_INSTANCES 1

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Usage: server port" << endl;
        return 1;
    }
    int port = atoi(argv[1]);
    if (port == 0)
    {
        cout << "Wrong port number" << endl;
        return 2;
    }

    EpollInstance ep;
    MemoryPool memoryPool;
    WordsCounter wordsCounter;
    EpollTcpServer server(port, ep, memoryPool, wordsCounter);
    ep.waitAndHandleEvents();

    // std::future<void> futures[EPOLL_INSTANCES];
    // for (int i = 0; i < EPOLL_INSTANCES; i++)
    // {
    //     std::packaged_task<void()> t(bind(&EpollInstance::waitAndHandleEvents, &ep));
    //     futures[i] = t.get_future();
    //     EpollTcpServer::pool.post(t);
    // }

    // for (int i = 0; i < EPOLL_INSTANCES; i++)
    // {
    //     futures[i].get();
    // }

    return 0;
}