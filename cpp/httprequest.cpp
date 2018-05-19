#include "HttpRequest.h"
#include <iostream>
#include <time.h>

using namespace std;

const char* HttpRequest::HTTP_OK = "200 OK";
const char* HttpRequest::HTTP_NO_CONTENT = "204 No Content";
const char* HttpRequest::HTTP_BAD_REQUEST = "400 Bad Request";
const char* HttpRequest::NEW_LINE = "\r\n";

HttpRequest::HttpRequest(MemoryPool & mp) : memoryPool(mp)
{
    receivedString = memoryPool.getCharBuffer();
}

HttpRequest::~HttpRequest()
{
    memoryPool.returnCharBuffer(receivedString);
}

void HttpRequest::print()
{
}

void HttpRequest::parseHeader()
{
    //cout << "Header parsed" << endl;
    //cout << "Data size: " << received.size() << endl;
    stringstream ss;
    for (auto it = received.begin(); it != received.end(); it++)
    {
        //cout << (char) *it;
        ss << *it;
    }
    //cout << endl;

    string line, temp, temp2, temp3;
    getline(ss, line);
    istringstream l1(line);

    // method
    getline(l1, temp, ' ');
    if (temp == "POST")
    {
        method = Method::POST;
        //cout << "method POST" << endl;
    }
    else if (temp == "GET")
    {
        method = Method::GET;
        //cout << "method GET" << endl;
    }

    // path
    getline(l1, temp, ' ');
    if (temp == "/esw/myserver/data")
    {
        command = Command::DATA;
        //cout << "command DATA" << endl;
    }
    else if (temp == "/esw/myserver/count")
    {
        command = Command::COUNT;
        //cout << "command COUNT" << endl;
    }

    while (getline(ss, temp, '\n'))
    {
        stringstream l2(temp);
        getline(l2, temp2, ':');
        getline(l2, temp3, ':');
        if (temp2 == "Content-Length")
        {
            contentSize = stoi(temp3.c_str());
            break;
        }
    }
    //cout << "Content size: " << contentSize << endl;

    received.clear();
}

void HttpRequest::addData(unsigned char * data, int len)
{
    int index = 0;

    if (!headerParsed)
    {
        if (index >= len)
            return;
        if (readBytes == 0) 
        {
            prev1 = data[index++];
            //received << prev1;
            received.push_back(prev1);
            readBytes++;
            if (index >= len)
                return;
        }
        if (readBytes == 1) 
        {
            prev2 = data[index++];
            //received << prev2;
            received.push_back(prev2);
            readBytes++;
            if (index >= len)
                return;
        }
        if (readBytes == 2) 
        {
            curr1 = data[index++];
            //received << curr1;
            received.push_back(curr1);
            readBytes++;
            if (index >= len)
                return;
        }

        for (; index < len; index++, readBytes++)
        {
            curr2 = data[index];
            //received << curr2;
            received.push_back(curr2);
            if (curr1 == 0x0d && curr2 == 0x0a
                    && prev1 == 0x0d && prev2 == 0x0a)
            {
                dataOffset = readBytes + 1;
                //cout << "Read bytes: " << readBytes << endl;
                //cout << "Data offset: " << dataOffset << endl;
                index++;
                parseHeader();
                headerParsed = true;
                break;   
            }
            prev1 = prev2;
            prev2 = curr1;
            curr1 = curr2;
        }
    }

    if (headerParsed)
    {
        //copy(data + index, data + len, back_inserter(received));
        copy(data + index, data + len, back_inserter(*receivedString));
        //received.write(data + index, len - index);
        readBytes += len;
        //cout << "Content size: " << contentSize << endl;
        //cout << "Read bytes: " << readBytes << endl;
        //cout << "Data offset: " << dataOffset << endl;
        if (readBytes - dataOffset >= contentSize) 
        {
            //cout << "Data parsed" << endl;
            dataParsed = true;
        }
    }
}

string HttpRequest::getResponse(const string & code, const string & data)
{
    char buf[100];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);

    stringstream ss;
    // Status Code
    ss << "HTTP/1.1 " << code << HttpRequest::NEW_LINE;

    // Date
    ss << "Date: " << buf << HttpRequest::NEW_LINE;

    // Content-Type
    ss << "Content-Type: text/plain";
    ss << HttpRequest::NEW_LINE;

    // Content-Length
    ss << "Content-Length: ";
    if (data.size() > 0)
        ss << data.size();
    else
        ss << "0";
    ss << HttpRequest::NEW_LINE;

    // Content
    if (data.size() > 0)
    {
        ss << HttpRequest::NEW_LINE;
        ss << data;
        ss << HttpRequest::NEW_LINE;
    }
    else
    {
        ss << HttpRequest::NEW_LINE;
    }

    return ss.str();
}