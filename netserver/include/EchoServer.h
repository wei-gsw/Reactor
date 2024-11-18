#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "TreadPool.h"
#include <signal.h>
class EchoServer
{
public:
    EchoServer(const string &ip, const uint16_t port, int threadnum = 3, int worksnum = 5);
    ~EchoServer();

    void start();
    void stop();

    // // 处理客户端连接请求
    void HannelNewConnection(spConnection conn);
    // 关闭客户端的连接，在Connection类中回调此函数,并从connections_中删除
    void HannelClose(spConnection conn);
    // 客户端连接出错，在Connection类中回调此函数,并从connections_中删除
    void HannelError(spConnection conn);
    // 处理客户端发送的消息，在Connection类中回调此函数
    void HannelMessage(spConnection conn, string &msg);
    // 发送完数据后的处理函数
    void HannelSendCompete(spConnection conn);
    // 处理epoll超时事件
    void HannelEpollTimeout(EventLoop *loop);
    // 业务处理的函数
    void OnMessage(spConnection conn, string &msg);

private:
    TcpServer tcpserver_;
    TreadPool threadpool_;
};