#include "WordsCounter.h"

#include <iostream>

using namespace std;

void WordsCounter::addWord(std::string & word)
{
    wordsMap.insert(make_pair(move(word), 1));
}

void WordsCounter::addWord2(size_t & hash)
{
    lock_guard<mutex> lock(_mapMutex);
    wordsMap2.insert(hash);
}

void WordsCounter::addWord3(size_t hash)
{
    wordsSet.insert(hash);
}

void WordsCounter::addWords2(vector<size_t> & hashes)
{
    lock_guard<mutex> lock(_mapMutex);
    for (auto it = hashes.begin(); it != hashes.end(); it++)
    {
        wordsMap2.insert(*it);
    }
}

void WordsCounter::addWords(std::vector<std::string> & words)
{
    lock_guard<mutex> lock(_mapMutex);
    for (auto it = words.begin(); it != words.end(); it++)
    {
        wordsMap.insert(make_pair(move(*it), 1));
    }
}

unsigned int WordsCounter::getWordCount()
{
    lock_guard<mutex> lock(_mapMutex);
    unsigned int size = wordsMap.size();
    wordsMap.clear();
    return size;
}

unsigned int WordsCounter::getWordCount2()
{
    lock_guard<mutex> lock(_mapMutex);
    unsigned int size = wordsMap2.size();
    wordsMap2.clear();
    return size;
}

unsigned int WordsCounter::getWordCount3()
{
    lock_guard<mutex> lock(_mapMutex);
    unsigned int size = wordsSet.size();
    wordsSet.clear();
    return size;
}