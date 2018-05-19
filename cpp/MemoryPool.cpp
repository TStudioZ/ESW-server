#include "MemoryPool.h"

using namespace std;

int MemoryPool::ARR_BUF_SIZE = 65536;

MemoryPool::MemoryPool()
{
    // for (int i = 0; i < 50; i++)
    // {
    //     arrayBuffers.push(createArrayBuffer());
    // }
    // for (int i = 0; i < 50; i++)
    // {
    //     charBuffers.push(createCharBuffer());
    // }
}

MemoryPool::~MemoryPool()
{
    // char * arrayBuffer = nullptr;
    // while (arrayBuffers.try_pop(arrayBuffer))
    // {
    //     delete[] arrayBuffer;
    // }

    // vector<char> * charBuffer = nullptr;
    // while (charBuffers.try_pop(charBuffer))
    // {
    //     delete charBuffer;
    // }
}

unsigned char * MemoryPool::createArrayBuffer()
{
    //cout << "Creating array buffer" << endl;
    return new unsigned char[ARR_BUF_SIZE];
}

vector<char> * MemoryPool::createCharBuffer()
{
    //cout << "Creating char buffer" << endl;
    vector<char> * buffer = new vector<char>();
    buffer->reserve(ARR_BUF_SIZE);
    return buffer;
}

unsigned char * MemoryPool::getArrayBuffer()
{
    return createArrayBuffer();
    // char * buffer = nullptr;
    // bool available = arrayBuffers.try_pop(buffer);
    // if (!available)
    // {
    //     buffer = createArrayBuffer();
    // }
    // //cout << "Getting array buffer" << endl;
    // return buffer;
}

vector<char> * MemoryPool::getCharBuffer()
{
    return createCharBuffer();
    // vector<char> * buffer = nullptr;
    // bool available = charBuffers.try_pop(buffer);
    // if (!available)
    // {
    //     buffer = createCharBuffer();
    // }
    // //cout << "Getting char buffer" << endl;
    // return buffer;
}

void MemoryPool::returnArrayBuffer(unsigned char * buffer)
{
    delete[] buffer;
    //cout << "Returning array buffer" << endl;
    //arrayBuffers.push(buffer);
}

void MemoryPool::returnCharBuffer(vector<char> * buffer)
{
    delete buffer;
    //cout << "Returning char buffer" << endl;
    // buffer->clear();
    // charBuffers.push(buffer);
}