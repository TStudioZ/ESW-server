Effective server in C++
Author: Tomáš Zahálka

The program is using the epoll mechanism and the following libraries:

thread-pool-cpp
https://github.com/inkooboo/thread-pool-cpp

Threading Building Blocks (Intel® TBB)
https://www.threadingbuildingblocks.org/

The Boost Iostreams Library
https://www.boost.org/doc/libs/1_67_0/libs/iostreams/doc/index.html



Makefile assumes the existence of listed libraries. Please modify it if needed.

To compile and link the program, do:
    make
To start the server, do:
    ./server [port]
