#include "wordscounter.h"
#include <iostream>

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