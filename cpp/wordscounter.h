#ifndef _WORDSCOUNTER_H_
#define _WORDSCOUNTER_H_

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

struct CustomHash
{
    size_t operator() (const size_t & key) const
    {
        return key;
    }
};

class WordsCounter
{
private:
    std::mutex _mapMutex;
    std::unordered_map<std::string, unsigned int> wordsMap;
    std::unordered_set<size_t> wordsMap2;
    tbb::concurrent_unordered_set<size_t, CustomHash> wordsSet;
public:
    WordsCounter() : wordsSet(300000)
    {
        //wordsSet = new tbb::concurrent_unordered_set<size_t, decltype(_hashFun)>(100, _hashFun);
    }
    ~WordsCounter()
    {
        //delete wordsSet;
    }
    void addWord(std::string &);
    void addWord2(size_t &);
    void addWord3(size_t);
    void addWords(std::vector<std::string> &);
    void addWords2(std::vector<size_t> &);
    unsigned int getWordCount();
    unsigned int getWordCount2();
    unsigned int getWordCount3();
    std::vector<std::future<void>> _futures;
};

#endif // _WORDSCOUNTER_H_