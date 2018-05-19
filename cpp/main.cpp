#include "EpollInstance.h"
#include "EpollTcpServer.h"
#include "WordsCounter.h"
#include "MemoryPool.h"

#include <iostream>
#include <sys/epoll.h>
#include <sys/timerfd.h>

using namespace std;

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

    return 0;
}