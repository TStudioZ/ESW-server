#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include "WordsCounter.h"
#include "MemoryPool.h"

#include <vector>
#include <sstream>

enum class Method
{
    GET,
    POST
};

enum class Command
{
    DATA,
    COUNT
};

class HttpRequest
{
private:
    MemoryPool & memoryPool;
    std::vector<char> * receivedString;
    std::vector<char> received;
    Method method;
    Command command;

    bool error = false;
    bool headerParsed = false;
    bool dataParsed = false;
    int readBytes = 0;
    int dataOffset = 0;
    int contentSize = 0;
    unsigned char prev1 = '\0', prev2 = '\0';
    unsigned char curr1 = '\0', curr2 = '\0';
public:
    static const char* HTTP_OK;
    static const char* HTTP_NO_CONTENT;
    static const char* HTTP_BAD_REQUEST;
    static const char* NEW_LINE;

    HttpRequest(MemoryPool &);
    ~HttpRequest();

    std::vector<char> & getReceivedString() { return *receivedString; }
    std::vector<char> & getReceived() { return received; }
    const Method & getMethod() const { return method; }
    const Command & getCommand() const { return command; }
    bool isError() const { return error; }
    bool isDataParsed() const { return dataParsed; }
    void print();
    void parseHeader();
    void addData(unsigned char*, int);
    static std::string getResponse(const std::string &, const std::string &);
};

#endif // _HTTP_REQUEST_H_