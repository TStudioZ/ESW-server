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
    //chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

    vector<char> & data = httpRequest->getReceivedString();
    //cout << "Data size: " << data.size() << endl;

    //vector<string> words;
    // vector<size_t> wordHashes;
    // wordHashes.reserve(100000);
    using namespace boost::iostreams;
    filtering_streambuf<input> in;
    in.push(gzip_decompressor());
    in.push(array_source(data.data(), data.size()));
    //boost::iostreams::copy(in, std::cout);
    std::istream instream(&in);
    string word;
    //vector<char> parsedData(std::istreambuf_iterator<char>{&in}, {});

    size_t h;
    while (instream >> word)
    {
        //cout << "[" << word << "]" << endl;
        h = hash<string>{}(word);
        //h = xxh::xxhash<64, char>((basic_string<char>) word);
        //wordHashes.push_back(h);
        wordsCounter.addWord3(h);
    }
    //wordsCounter.addWords2(wordHashes);
    
    //vector<char> parsedData;
    //string dataStr(data.begin(), data.end());
    //cout << dataStr << endl;
    //utf8::iterator<char*> it(data.begin(), data.begin(), data.end());

    // auto itBegin = parsedData.begin(), itEnd = parsedData.begin();
    // for (; itEnd != parsedData.end(); itEnd++)
    // {
    //     if (isspace(*itBegin))
    //     {
    //         itBegin++;
    //     }
    //     else if (isspace(*itEnd))
    //     {
    //         string dataStr(itBegin, itEnd);
    //         //cout << "[" << dataStr << "]" << endl;
    //         h = hash<string>{}(dataStr);
    //         wordsCounter.addWord3(h);
    //         itBegin = itEnd + 1;
    //         itEnd++;
    //     }
    // }
    // if (itBegin != itEnd)
    // {
    //     string dataStr(itBegin, itEnd);
    //     //cout << "[" << dataStr << "]" << endl;
    //     h = hash<string>{}(dataStr);
    //     wordsCounter.addWord3(h);
    // }

    // chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    // auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    // cout << "Processing time (ms): " << diff << endl;
    delete this;
}