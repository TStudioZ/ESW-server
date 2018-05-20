#include "MemoryPool.h"

using namespace std;

int MemoryPool::ARR_BUF_SIZE = 65536;

MemoryPool::MemoryPool()
{
}

MemoryPool::~MemoryPool()
{
}

unsigned char * MemoryPool::createArrayBuffer()
{
    return new unsigned char[ARR_BUF_SIZE];
}

vector<char> * MemoryPool::createCharBuffer()
{
    vector<char> * buffer = new vector<char>();
    buffer->reserve(ARR_BUF_SIZE);
    return buffer;
}

unsigned char * MemoryPool::getArrayBuffer()
{
    return createArrayBuffer();
}

vector<char> * MemoryPool::getCharBuffer()
{
    return createCharBuffer();
}

void MemoryPool::returnArrayBuffer(unsigned char * buffer)
{
    delete[] buffer;
}

void MemoryPool::returnCharBuffer(vector<char> * buffer)
{
    delete buffer;
}