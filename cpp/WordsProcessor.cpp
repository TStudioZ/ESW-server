#include "WordsProcessor.h"

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>

using namespace std;

WordsProcessor::WordsProcessor(HttpRequest * hr, MemoryPool & mp, WordsCounter & wc) 
    : httpRequest(hr), memoryPool(mp), wordsCounter(wc)
{
}

WordsProcessor::~WordsProcessor()
{
    delete httpRequest;
}

void WordsProcessor::processWords()
{
    chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

    vector<char> & data = httpRequest->getReceivedString();
    using namespace boost::iostreams;
    filtering_streambuf<input> in;
    in.push(gzip_decompressor());
    in.push(array_source(data.data(), data.size()));
    std::istream instream(&in);
    string word;

    size_t h;
    while (instream >> word)
    {
        h = hash<string>{}(word);
        wordsCounter.addWord(h);
    }

    chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    cout << "Processing time (ms): " << diff << endl;
    delete this;
}