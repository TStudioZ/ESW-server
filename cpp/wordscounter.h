#ifndef _WORDS_COUNTER_H_
#define _WORDS_COUNTER_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <thread>
#include <future>
#include <functional>
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_unordered_set.h>

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
    WordsCounter();
    ~WordsCounter();
    void addWord(size_t);
    unsigned int getWordCount();
    std::vector<std::future<void>> _futures;
};

#endif // _WORDS_COUNTER_H_