#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <thread>
#include <future>
#include <functional>
#include <memory>
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_unordered_set.h>

class MemoryPool
{
private:
    tbb::concurrent_queue<unsigned char*> arrayBuffers;
    tbb::concurrent_queue<std::vector<char>*> charBuffers;
    unsigned char * createArrayBuffer();
    std::vector<char> * createCharBuffer();
public:
    static int ARR_BUF_SIZE;

    MemoryPool();
    ~MemoryPool();
    unsigned char * getArrayBuffer();
    std::vector<char> * getCharBuffer();
    void returnArrayBuffer(unsigned char*);
    void returnCharBuffer(std::vector<char> *);
};

#endif // _MEMORY_POOL_H_