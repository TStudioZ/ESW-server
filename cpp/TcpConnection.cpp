#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cctype>
#include <sstream>
#include <vector>
#include <iterator>
#include <thread>
#include <future>
#include <functional>

#include "TcpConnection.h"
#include "WordsProcessor.h"

using namespace std;

tp::ThreadPool TcpConnection::pool;

TcpConnection::TcpConnection(int cfd, EpollInstance &e, MemoryPool & mp, WordsCounter & wc) 
    : EpollFd(cfd, e), httpRequest(new HttpRequest(mp)), length(0), memoryPool(mp), wordsCounter(wc)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        printError();
        return;
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1)
    {
        printError();
        return;
    }
    registerFd(EPOLLIN | EPOLLONESHOT);

    buffer = memoryPool.getArrayBuffer();
}

TcpConnection::~TcpConnection()
{
    memoryPool.returnArrayBuffer(buffer);
}

void TcpConnection::handleEvent(uint32_t events)
{
    if (events & EPOLLERR)
    {
        closeConnection();
    }
    else if (events & EPOLLIN)
    {
        if (!readData())
        {
            closeConnection();
        }
    }
    else if (events & EPOLLOUT)
    {
        if (!writeData())
        {
            closeConnection();
        }
    }
    else
    {
        closeConnection();
    }
}

bool TcpConnection::processRequest()
{
    httpRequest->print();

    const Command & command = httpRequest->getCommand();
    const Method & method = httpRequest->getMethod();

    if (!httpRequest->isError())
    {
        if (method == Method::POST && command == Command::DATA)
        {
            WordsProcessor * wordsProcessor = new WordsProcessor(httpRequest, memoryPool, wordsCounter);
            std::packaged_task<void()> t(bind(&WordsProcessor::processWords, wordsProcessor));
            wordsCounter._futures.push_back(t.get_future());
            TcpConnection::pool.post(t);

            statusCode = HttpRequest::HTTP_NO_CONTENT;
            response = "OK";
        }
        else if (method == Method::GET && command == Command::COUNT)
        {
            for_each(wordsCounter._futures.begin(), wordsCounter._futures.end(), [](std::future<void> & f) {
                f.get();
            });
            wordsCounter._futures.clear();

            statusCode = HttpRequest::HTTP_OK;
            response = to_string(wordsCounter.getWordCount());

            delete httpRequest;
        }
        else
        {
            statusCode = HttpRequest::HTTP_BAD_REQUEST;
            response = "Unknown command";

            delete httpRequest;
        }
    }
    else
    {
        statusCode = HttpRequest::HTTP_BAD_REQUEST;
        response = "Bad request";

        delete httpRequest;
    }

    updateFd(EPOLLOUT);
    return true;
}

bool TcpConnection::readData()
{
    int readCount;

    while (true)
    {
        readCount = read(fd, buffer, MemoryPool::ARR_BUF_SIZE);
        if (readCount == 0)
        {
            return false;
        }
        if (readCount == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                if (httpRequest->isDataParsed())
                {
                    return processRequest();
                }
                else
                {
                    updateFd(EPOLLIN | EPOLLONESHOT);
                }
                return true;
            }
            else
            {
                printError();
                return false;
            }
        }

        if (readCount > 0)
        {
            httpRequest->addData(buffer, readCount);
        }
    }
}

bool TcpConnection::writeData()
{
    std::string out = HttpRequest::getResponse(statusCode, response);
    int writeCount = write(fd, out.c_str(), out.length());
    if (writeCount == -1)
    {
        printError();
        return false;
    }
    return false;
}

void TcpConnection::closeConnection()
{
    unregisterFd();
    if (close(fd) == -1)
    {
        printError();
    }
    delete this;
}
