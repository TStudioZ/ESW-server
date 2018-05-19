#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#include "EpollFd.h"
#include "HttpRequest.h"
#include "WordsCounter.h"

#include <string>
#include <thread_pool.hpp>

class TcpConnection : public EpollFd
{
public:
    TcpConnection(int cfd, EpollInstance &e, MemoryPool &, WordsCounter &);
    ~TcpConnection();
    void handleEvent(uint32_t events);
    static tp::ThreadPool pool;
private:
    unsigned char * buffer;
    HttpRequest * httpRequest;
    int length;
    MemoryPool & memoryPool;
    WordsCounter & wordsCounter;
    std::string statusCode;
    std::string response;
    
    bool processRequest();
    bool readData();
    bool writeData();
    void closeConnection();
};

#endif // _TCP_CONNECTION_H_