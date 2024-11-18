#pragma once
#include "Socket.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Buffer.h"
#include <cstring>
#include <functional>
#include <memory>
#include<sys/syscall.h> // for SYS_gettid call
#include <atomic>
#include "Timestamp.h"

class EventLoop; // 前向声明
class Channel;   // 前向声明

class Connection;                                 // 前向声明
using spConnection = std::shared_ptr<Connection>; // 定义智能指针
//  接受数据的fd可以封装成Connector类，
class Connection : public std::enable_shared_from_this<Connection>
{
private:
    EventLoop *loop_; // Connection 所属的事件循环,从构造函数中传入
    std::unique_ptr<Socket> clientsock_;     // 与客户端通信的 Socket
    std::unique_ptr<Channel> clientchannel_; // Acceptor 所创建的用于监听新连接的 Channel，在构造函数中创建

    std::function<void(spConnection)> closecallback_;          // 关闭连接fd的回调函数,将回调TcpServer的closeconnection函数
    std::function<void(spConnection)> errorcallback_;          // 错误回调函数
    function<void(spConnection, string &)> onmessagecallback_; // 处理接收到消息的回调函数
    function<void(spConnection)> sendcompletecallback_;        // 发送完成回调函数

    Buffer inputbuffer_;            // 接受缓冲区
    Buffer outputbuffer_;           // 发送缓冲区
    std::atomic_bool disconnected_; // 连接是否已经断开
    Timestamp lastatime_;          // 上次活跃时间

public:
    Connection(EventLoop *loop, std::unique_ptr<Socket> clientsock);
    ~Connection();

    // 返回clientsock_fd
    int fd() const;
    // 返回clientsock_的InetAddress
    std::string ip() const;
    // 返回 clientsock_的port
    uint16_t port() const;

    // 处理客户端发送过来的信息
    void onmessage();

    // 关闭连接的回调函数,供Channel调用
    void closecallback();
    // 错误回调函数，供Channel调用
    void errorcallback();
    //  写事件回调回调函数，供Channel调用
    void writecallback();

    // 设置关闭连接的回调函数,供TcpServer调用
    void setclosecallback(const std::function<void(spConnection)> &cb);
    // 设置错误回调函数,供TcpServer调用
    void seterrorcallback(const std::function<void(spConnection)> &cb);

    // 设置处理客户端消息的回调函数
    void setonmessagecallback(function<void(spConnection, string &)> fn);

    // 设置发送完成回调函数
    void setsendcompletecallback(function<void(spConnection)> fn);
    // 发送消息到客户端,不论什么线程都调用
    void send(const char *data, size_t len);
    // 发送消息到客户端，在loop线程中调用
    void sendinloop(const char *data, size_t len);

    bool timeout(time_t now, int val);
};
