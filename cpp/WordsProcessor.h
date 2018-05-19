#ifndef _WORDS_PROCESSOR_
#define _WORDS_PROCESSOR_

#include "HttpRequest.h"
#include "MemoryPool.h"
#include "WordsCounter.h"

class WordsProcessor
{
private:
    HttpRequest * httpRequest;
    MemoryPool & memoryPool;
    WordsCounter & wordsCounter;
public:
    WordsProcessor(HttpRequest*, MemoryPool&, WordsCounter&);
    ~WordsProcessor();
    void processWords();
};

#endif // _WORDS_PROCESSOR_