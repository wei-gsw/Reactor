#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <strings.h>
#include <string>
#include <errno.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <vector>
#include "Channel.h"
using namespace std;

class Channel; // 前向声明 Channel 类

class Epoll
{
private:
    // epoll_wait返回的evs数组大小
    static const int MaxEvents = 100;
    // epoll句柄
    int epollfd_ = -1;
    // epollwait返回数组，在构造函数里分配内存
    epoll_event events_[MaxEvents];

public:
    // 构造epollfd
    Epoll();
    //  关闭epollfd
    ~Epoll();
    // 将channel和需要监视的事件挂到红黑树上
    void updateChannel(Channel *ch);
    // 从红黑树上移除channel
    void removeChannel(Channel *ch);
    // 运行epollwait，将以发生的事件用vector返回
    vector<Channel *> loop(int timeout = -1);
};