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
#include <ctime>
#include <chrono>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>

#include "epolltcpserver.h"

using namespace std;

void printError()
{
    std::cerr << "Error: " << std::strerror(errno) << std::endl;
}

tp::ThreadPool EpollTcpServer::pool;
int EpollTcpServer::HANDLERS_COUNT = 1;

EpollTcpServer::EpollTcpServer(uint32_t p, EpollInstance &e, MemoryPool & mp, WordsCounter & wc) 
    : EpollFd(-1, e), port(p), memoryPool(mp), wordsCounter(wc)
{
    if (startServer())
    {
        serverStarted = true;
        registerFd(EPOLLIN);
    }
    else
    {
        serverStarted = false;
    }

    // thread t(bind(&EpollInstance::waitAndHandleEvents, &ep));
    // t.detach();

    // rr = 0;
    // eps = new EpollInstance[EpollTcpServer::HANDLERS_COUNT];
    // for (int i = 0; i < EpollTcpServer::HANDLERS_COUNT; i++)
    // {
    //     //std::packaged_task<void()> t(bind(&EpollInstance::waitAndHandleEvents, &ep));
    //     //EpollTcpServer::pool.post(t);
    //     thread t(bind(&EpollInstance::waitAndHandleEvents, &eps[i]));
    //     t.detach();
    // }
}

EpollTcpServer::~EpollTcpServer()
{
    //delete[] eps;

    if (serverStarted)
    {
        unregisterFd();
    }
}

void EpollTcpServer::handleEvent(uint32_t events)
{
    if (events & EPOLLERR)
    {
        //cout << "Unregistering server fd..." << endl;
        unregisterFd();
    }
    else if (events & EPOLLIN)
    {
        acceptConnection();
    }
    else
    {
        unregisterFd();
    }
}

bool EpollTcpServer::startServer()
{
    //std::cout << "Starting server on port: " << port << std::endl;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        printError();
        return false;
    }

    struct sockaddr_in saddr;

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family      = AF_INET;              // IPv4
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);    // Bind to all available interfaces
    saddr.sin_port        = htons(port);          // Requested port

    int r = bind(fd, (struct sockaddr *) &saddr, sizeof(saddr));
    if (r == -1)
    {
        printError();
        return false;
    }

    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        printError();
        return false;
    }
    flags |= O_NONBLOCK;
    r = fcntl(fd, F_SETFL, flags);
    if (r == -1)
    {
        printError();
        return false;
    }

    if (listen(fd, SOMAXCONN) == -1)
    {
        printError();
        return false;
    }

    std::cout << "Server listening on port: " << port << std::endl;
    return true;
}

void EpollTcpServer::acceptConnection()
{
    //cout << "Accepting connection...";
    int cfd = accept(fd, NULL, NULL);
    if (cfd == -1)
    {
        printError();
        return;
    }
    new TcpConnection(cfd, epollInstance, memoryPool, wordsCounter);
    //rr = (rr + 1) % EpollTcpServer::HANDLERS_COUNT;
    //std::cout << "Connection established" << std::endl;
}

TcpAcceptor::TcpAcceptor(int socketFd, EpollInstance & e, MemoryPool & mp, WordsCounter & wc) 
    : EpollFd(socketFd, e), memoryPool(mp), wordsCounter(wc)
{
    registerFd(EPOLLIN);
}

TcpAcceptor::~TcpAcceptor()
{
    unregisterFd();
}

void TcpAcceptor::handleEvent(uint32_t events)
{
    if ((events & EPOLLERR))
    {
        //cout << "Unregistering acceptor fd..." << endl;
        unregisterFd();
    }
    else if (events & EPOLLIN)
    {
        acceptConnection();
    }
    else
    {
        unregisterFd();
    }
}

void TcpAcceptor::acceptConnection()
{
    int cfd = accept(fd, NULL, NULL);
    if (cfd == -1)
    {
        printError();
        return;
    }
    new TcpConnection(cfd, epollInstance, memoryPool, wordsCounter);
    //std::cout << "Connection established" << std::endl;
}

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
        //cout << "Closing on error..." << endl;
        closeConnection();
    }
    else if (events & EPOLLIN)
    {
        //cout << "Reading..." << endl;
        if (!readData())
        {
            closeConnection();
        }
    }
    else if (events & EPOLLOUT)
    {
        //cout << "Writing..." << endl;
        if (!writeData())
        {
            closeConnection();
        }
    }
    else
    {
        //cout << "Closing..." << endl;
        closeConnection();
    }
}

WordsProcessor::WordsProcessor(HttpRequest * hr, MemoryPool & mp, WordsCounter & wc) 
    : httpRequest(hr), memoryPool(mp), wordsCounter(wc)
{
}

WordsProcessor::~WordsProcessor()
{
    delete httpRequest;
}

void WordsProcessor::processWords()
{
    vector<char> & data = httpRequest->getReceivedString();
    //cout << "Data size: " << data.size() << endl;

    //vector<string> words;
    // vector<size_t> wordHashes;
    // wordHashes.reserve(100000);
    using namespace boost::iostreams;
    filtering_streambuf<input> in;
    in.push(gzip_decompressor());
    in.push(array_source(data.data(), data.size()));
    //boost::iostreams::copy(in, std::cout);
    std::istream instream(&in);
    string word;
    //vector<char> parsedData(std::istreambuf_iterator<char>{&in}, {});

    size_t h;
    while (instream >> word)
    {
        //cout << "[" << word << "]" << endl;
        h = hash<string>{}(word);
        //h = xxh::xxhash<64, char>((basic_string<char>) word);
        //wordHashes.push_back(h);
        wordsCounter.addWord3(h);
    }
    //wordsCounter.addWords2(wordHashes);
    
    //vector<char> parsedData;
    //string dataStr(data.begin(), data.end());
    //cout << dataStr << endl;
    //utf8::iterator<char*> it(data.begin(), data.begin(), data.end());

    // auto itBegin = parsedData.begin(), itEnd = parsedData.begin();
    // for (; itEnd != parsedData.end(); itEnd++)
    // {
    //     if (isspace(*itBegin))
    //     {
    //         itBegin++;
    //     }
    //     else if (isspace(*itEnd))
    //     {
    //         string dataStr(itBegin, itEnd);
    //         //cout << "[" << dataStr << "]" << endl;
    //         h = hash<string>{}(dataStr);
    //         wordsCounter.addWord3(h);
    //         itBegin = itEnd + 1;
    //         itEnd++;
    //     }
    // }
    // if (itBegin != itEnd)
    // {
    //     string dataStr(itBegin, itEnd);
    //     //cout << "[" << dataStr << "]" << endl;
    //     h = hash<string>{}(dataStr);
    //     wordsCounter.addWord3(h);
    // }
    delete this;
}

bool TcpConnection::processRequest()
{
    //std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    httpRequest->print();

    const Command & command = httpRequest->getCommand();
    const Method & method = httpRequest->getMethod();

    if (!httpRequest->isError())
    {
        if (method == Method::POST && command == Command::DATA)
        {
            //cout << "Processing POST DATA..." << endl;

            //chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
            WordsProcessor * wordsProcessor = new WordsProcessor(httpRequest, memoryPool, wordsCounter);
            std::packaged_task<void()> t(bind(&WordsProcessor::processWords, wordsProcessor));
            wordsCounter._futures.push_back(t.get_future());
            EpollTcpServer::pool.post(t);
            // processWords();
            // chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            // auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
            // cout << "Processing time (ms): " << diff << endl;

            statusCode = HttpRequest::HTTP_NO_CONTENT;
            response = "OK";
        }
        else if (method == Method::GET && command == Command::COUNT)
        {
            //cout << "Processing GET COUNT..." << endl;

            for_each(wordsCounter._futures.begin(), wordsCounter._futures.end(), [](std::future<void> & f) {
                f.get();
            });
            wordsCounter._futures.clear();

            statusCode = HttpRequest::HTTP_OK;
            //response = "0";
            response = to_string(wordsCounter.getWordCount3());

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
    //writeData();
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
            //std::cout << "End of message" << std::endl;
            return false;
        }
        if (readCount == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                if (httpRequest->isDataParsed())
                {
                    //std::packaged_task<bool()> t(processRequest);
                    //std::packaged_task<bool()> t(bind(&TcpConnection::processRequest, this));
                    //EpollTcpServer::pool.post(t);
                    return processRequest();
                    //thread t(bind(&TcpConnection::processRequest, this));
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
            //std::cout << "Receive count: " << readCount << std::endl;
            httpRequest->addData(buffer, readCount);
        }
    }
}

bool TcpConnection::writeData()
{
    std::string out = HttpRequest::getResponse(statusCode, response);
    //cout << out;
    int writeCount = write(fd, out.c_str(), out.length());
    //cout << "Response length: " << out.size() << ", written count: " << writeCount << endl;
    if (writeCount == -1)
    {
        printError();
        return false;
    }

    //updateFd(EPOLLIN | EPOLLET);
    //return true;
    return false;
}

void TcpConnection::closeConnection()
{
    //std::cout << "Unregistering connection" << std::endl;
    unregisterFd();
    if (close(fd) == -1)
    {
        printError();
    }
    delete this;
}
