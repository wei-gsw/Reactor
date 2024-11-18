#pragma once
#include "Socket.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include "Channel.h"
#include <functional>
#include "Connection.h"
#include <memory>

class Acceptor
{
private:
    // Acceptor.h 文件中的 Acceptor 类所需的成员变量
    EventLoop *loop_;                                                    // 事件循环,从构造函数中传入
    Socket servsock_;                                                    // 服务器用于监听的socket，在构造函数中创建
    Channel accepetchannel_;                                             // Acceptor 所创建的用于监听新连接的 Channel，在构造函数中创建
    std::function<void(std::unique_ptr<Socket>)> newconnectioncallback_; // 处理客户端连接的回调函数，将指向TcpConnection类的对象作为参数传入
public:
    Acceptor(EventLoop *loop, string ip, const uint16_t port);
    ~Acceptor();
    // 调用Acceptor后生成的新连接的回调函数,处理客户端的连接请求
    void newconnection();
    // 处理客户端连接请求
    void setnewconnectioncallback(std::function<void(std::unique_ptr<Socket>)> callback);
};
