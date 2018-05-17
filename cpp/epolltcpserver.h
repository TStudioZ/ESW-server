#ifndef EPOLLTCPSERVER_H
#define EPOLLTCPSERVER_H

#include "epollfd.h"
#include "httprequest.h"
#include "wordscounter.h"
#include <string>
#include <thread_pool.hpp>
#include <thread>
#include <future>
#include <functional>
#include <memory>

class EpollTcpServer : public EpollFd
{
public:
    EpollTcpServer(uint32_t port, EpollInstance &e, MemoryPool&, WordsCounter&);
    ~EpollTcpServer();
    void handleEvent(uint32_t events);
    static tp::ThreadPool pool;
private:
    static int HANDLERS_COUNT;
    int rr;
    //EpollInstance * eps;
    //EpollInstance ep;
    
    uint32_t port;
    bool serverStarted;
    MemoryPool & memoryPool;
    WordsCounter & wordsCounter;
    bool startServer();
    void acceptConnection();
};

class TcpAcceptor : public EpollFd
{
private:
    MemoryPool & memoryPool;
    WordsCounter & wordsCounter;
public:
    TcpAcceptor(int, EpollInstance &, MemoryPool&, WordsCounter&);
    ~TcpAcceptor();
    void handleEvent(uint32_t events);
    void acceptConnection();
};

class TcpConnection : public EpollFd
{
public:
    TcpConnection(int cfd, EpollInstance &e, MemoryPool &, WordsCounter &);
    ~TcpConnection();
    void handleEvent(uint32_t events);
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

class WordsProcessor
{
private:
    HttpRequest * httpRequest;
    MemoryPool & memoryPool;
    WordsCounter & wordsCounter;
public:
    WordsProcessor(HttpRequest*, MemoryPool&, WordsCounter&);
    ~WordsProcessor();
    void processWords();
};

#endif // EPOLLTCPSERVER_H
