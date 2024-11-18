#include "Epoll.h"
#include "Channel.h" // 确保包含 Channel.h

// 构造epollfd
Epoll::Epoll()
{ // 创建epoll句柄
    epollfd_ = epoll_create(1);
    if (epollfd_ == -1)
    {
        perror("epoll_create failed!");
        exit(-1);
    }
}

//  关闭epollfd
Epoll ::~Epoll()
{
    close(epollfd_);
}

// 更新channel
void Epoll::updateChannel(Channel *ch)
{
    epoll_event ev;
    ev.data.ptr = ch;
    ev.events = ch->events();
    if (ch->inepoll())
    {
        if ((epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->fd(), &ev)) == -1)
        {
            perror("epoll_ctl failed!");
            std::cout << "errorfd:" << ch->fd();
            exit(-1);
        }
    }
    else
    {
        if ((epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->fd(), &ev)) == -1)
        {
            perror("epoll_ctl failed!");
            std::cout << "errorfd:" << ch->fd();
            exit(-1);
        }
        ch->setinepoll();
    }
}

// 从红黑树上移除channel
void Epoll::removeChannel(Channel *ch)
{
    if (ch->inepoll())
    {
        if ((epoll_ctl(epollfd_, EPOLL_CTL_DEL, ch->fd(), 0)) == -1)
        {
            
            perror("epoll_ctl failed!");
            std::cout << "errorfd:" << ch->fd();
            exit(-1);
        }
    }
}

// 运行epollwait，将以发生的事件用vector返回
vector<Channel *> Epoll::loop(int timeout)
{
    vector<Channel *> chs;
    bzero(events_, sizeof(events_));
    int infds = epoll_wait(epollfd_, events_, MaxEvents, timeout);
    //  当 infds 小于零时，表明在调用 epoll_wait 的过程中发生了错误。这可能是由于无效的文件描述符或内部错误等原因引起的。
    if (infds < 0)
    {
        perror("epoll_wait failed!");
        exit(-1);
    }
    // 当 infds 等于零时，表示在指定的超时时间内没有任何事件发生。这是正常的超时情况，可能是在没有任何连接活动的情况下发生的。返回空的channel
    if (infds == 0)
    {
        // perror("epoll_wait timeout!");
        return chs;
    }
    // intfd>0表示有事件发生的个数
    for (int i = 0; i < infds; ++i)
    {
        Channel *ch = (Channel *)events_[i].data.ptr;
        ch->setrevents(events_[i].events);
        chs.push_back(ch);
    }

    return chs;
}