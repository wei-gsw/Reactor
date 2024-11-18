#pragma once
#include <vector>
#include <cstdio>
#include <bits/types.h>
#include <sys/epoll.h>
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include <functional>
#include "EventLoop.h"
#include <memory>
#include "Buffer.h"
#include "Connection.h"
using namespace std;

class Epoll; // 前向声明 Epoll 类
class EventLoop;
class Channel
{
private:
    // ch拥有的fd ，一对一
    int fd_ = -1;
    // ch所属的EventLoop,channel和EventLoop是一对一的关系
    EventLoop *loop_ = nullptr;
    // ch是否在epoll树上，不在调用epoll_ctl的时候用EPOLL_CTL_ADD，否则，EPOLL_CTL_MOD
    bool inepoll_ = false;
    // fd监听的事件
    uint32_t events_ = 0;
    // fd已发生的事件
    uint32_t revents_ = 0;
    // fd读事件的回调
    function<void()> readcallback_;
    function<void()> closecallback_;
    function<void()> errorcallback_;
    function<void()> writecallback_;

public:
    Channel(EventLoop *loop, int fd);

    ~Channel();

    // 返回fd
    int fd();
    // 采用边沿触发
    void useet();
    // 让epoll_wait监听读事件,注册读事件
    void enablereading();
    // 取消读事件
    void disablereading();
    // 注册写事件
    void enablewriting();
    // 取消写事件
    void disablewriting();

    // 把inepoll设为true
    void setinepoll();
    // 设置revents的成员参数为ev
    void setrevents(uint32_t ev);
    // 返回inepoll_
    bool inepoll();
    // 返回events_
    uint32_t events();
    // 返回revents_
    uint32_t revents();
    // 事件处理函数，epollwait返回时执行
    void handleevent();

    // 处理客户端连接请求
    void newconnection(Socket *servsock);

    // 设置fd读事件的回调函数
    void setreadcallback(function<void()> fn);
    // 设置fd关闭事件的回调函数，将回调Connection::closecallback_
    void setclosecallback(function<void()> fn);
    // 设置fd错误事件的回调函数,将回调Connection::errorcallback_
    void seterrorcallback(function<void()> fn);

    //    // 设置fd写事件的回调函数
    void setwritecallback(function<void()> fn);

    void remove();     // 移除channel
    void disableall(); // 禁用所有事件
};