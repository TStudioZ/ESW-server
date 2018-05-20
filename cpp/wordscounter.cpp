#include "WordsCounter.h"

#include <iostream>

using namespace std;

WordsCounter::WordsCounter() : wordsSet(500000)
{
}

WordsCounter::~WordsCounter()
{
}

void WordsCounter::addWord(size_t hash)
{
    wordsSet.insert(hash);
}

unsigned int WordsCounter::getWordCount()
{
    lock_guard<mutex> lock(_mapMutex);
    unsigned int size = wordsSet.size();
    wordsSet.clear();
    return size;
}