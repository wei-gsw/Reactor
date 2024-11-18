#include "Channel.h"
#include <string.h>
#include "Socket.h"
#include "Epoll.h" // 确保包含 Epoll.h
#include "EventLoop.h"

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd)
{
}

Channel::~Channel() {};

// 返回fd
int Channel::fd()
{
    return fd_;
}

// 采用边沿触发
void Channel::useet()
{
    events_ = events_ | EPOLLET;
}

// 让epoll_wait监听读事件
void Channel::enablereading()
{
    events_ = events_ | EPOLLIN;
    loop_->updateChannel(this);
}

// 取消读事件
void Channel::disablereading()
{
    events_ = events_ & (~EPOLLIN);
    loop_->updateChannel(this);
}

// 注册写事件
void Channel::enablewriting()
{
    events_ = events_ | EPOLLOUT;
    loop_->updateChannel(this);
}

// 取消写事件
void Channel::disablewriting()
{
    events_ = events_ & (~EPOLLOUT);
    loop_->updateChannel(this);
}

// 把inepoll设为true
void Channel::setinepoll()
{
    inepoll_ = true;
}
// 设置revents的成员参数为ev
void Channel::setrevents(uint32_t ev)
{
    revents_ = ev;
}
// 返回inepoll_
bool Channel::inepoll()
{
    return inepoll_;
}
// 返回events_
uint32_t Channel::events()
{
    return events_;
}
// 返回revents_
uint32_t Channel::revents()
{
    return revents_;
}

void Channel::handleevent()
{
    // 对方已关闭，有些系统检测不到
    if (revents_ & EPOLLRDHUP)
    {
        // cout << "client(fd=" << fd_ << ") error!" << endl;
        // close(fd_);

        // cout << "EPOLLRDHUP" << endl;
        //  关闭连接
        closecallback_();
    }
    // 缓冲区有数据可以读
    else if (revents_ & (EPOLLIN | EPOLLERR))
    {
        // cout << "EPOLLIN | EPOLLERR" << endl;
        //  // 表示有连接上来
        //  if (islisten_ == true)
        //      newconnection(servsock);
        //  else // 客户端fd有事件
        //      onmessage();
        readcallback_();
    }
    // 写事件，有数据要写
    else if (revents_ & EPOLLOUT)
    {
        // cout << "EPOLLOUT" << endl;

        // 写缓冲区有数据，开始写
        writecallback_();
    }
    else
    {
        // cout << "other event" << endl;
        //  其他事件，视为错误断开连接
        //  cout << "client(fd=" << fd_ << ") error!" << endl;
        //  close(fd_);
        errorcallback_();
    }
}

// 设置fd读事件的回调函数
void Channel::setreadcallback(function<void()> fn)
{
    readcallback_ = fn;
}

// 设置fd关闭事件的回调函数，将回调Connection::closecallback_
void Channel::setclosecallback(function<void()> fn)
{
    closecallback_ = fn;
}

// 设置fd错误事件的回调函数,将回调Connection::errorcallback_
void Channel::seterrorcallback(function<void()> fn)
{
    errorcallback_ = fn;
}

void Channel::setwritecallback(function<void()> fn)
{
    writecallback_ = fn;
}

// 移除channel
void Channel::remove()
{
    loop_->removeChannel(this);
}
// 禁用所有事件
void Channel::disableall()
{

    events_ = 0;
    loop_->updateChannel(this);
}