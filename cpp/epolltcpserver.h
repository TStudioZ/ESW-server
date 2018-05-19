#ifndef _EPOLL_TCP_SERVER_H_
#define _EPOLL_TCP_SERVER_H_

#include "EpollFd.h"
#include "HttpRequest.h"
#include "WordsCounter.h"

class EpollTcpServer : public EpollFd
{
public:
    EpollTcpServer(uint32_t port, EpollInstance &e, MemoryPool&, WordsCounter&);
    ~EpollTcpServer();
    void handleEvent(uint32_t events);
private:
    uint32_t port;
    bool serverStarted;
    MemoryPool & memoryPool;
    WordsCounter & wordsCounter;
    bool startServer();
    void acceptConnection();
};

#endif // _EPOLL_TCP_SERVER_H_
