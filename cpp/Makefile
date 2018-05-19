CXX=g++
CPPFLAGS=-std=c++1z -g -Wall -pthread -o3
LDLIBS=-lz -lboost_iostreams -ltbb -L/root/tbb2018_20180312oss/lib/intel64/gcc4.7/
INCLUDES=-I. -Ithread_pool -I/root/tbb2018_20180312oss/include/
SOURCES=main.cpp EpollFd.cpp EpollInstance.cpp EpollTcpServer.cpp TcpConnection.cpp HttpRequest.cpp WordsCounter.cpp MemoryPool.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: server

server: $(SOURCES)
	$(CXX) $(CPPFLAGS) $(INCLUDES) -o $@ $(SOURCES) $(LDLIBS)

.cpp.o:
	$(CXX) $(CPPFLAGS) -c $<

clean:
	rm $(OBJS) server