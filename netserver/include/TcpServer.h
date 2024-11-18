#pragma once
#include "EventLoop.h"
#include "Channel.h"
#include "Socket.h"
#include "Acceptor.h"
#include "Connection.h"
#include <map>
#include <string>
#include <mutex>
#include "TreadPool.h"
// 网络服务器类
// 监听的fd可以封装成Acceptor类，
// 接受数据的fd可以封装成Connector类，
class TcpServer
{
private:
    // 一个TcpServer可以有多个事件循环，现在是单线程，所以只有一个
    std::unique_ptr<EventLoop> mainloop_;         // 主线程
    vector<std::unique_ptr<EventLoop>> subloops_; // 从线程容器
    int threadnum_;
    int worksnum_;
    // 监听的fd, 封装成Acceptor类,一个TcpServer只能有一个Acceptor
    Acceptor acceptor_;
    TreadPool threadpool_; // 线程池
    // 存放所有连接的fd和对应的Connection类
    map<int, spConnection> connections_;

    std::mutex connections_mutex_;

    //   回调函数
    function<void(spConnection)> newconnectioncb_;
    function<void(spConnection)> closeconnectioncb_;
    function<void(spConnection)> errorconnectioncb_;
    function<void(spConnection, string &)> onmessagecb_;
    function<void(spConnection)> sendcompetecb_;
    function<void(EventLoop *)> epolltimeoutcb_;
    

public:
    TcpServer(const string &ip, const uint16_t port, int threadnum = 3);
    ~TcpServer();
    // 启动TcpServer
    void start();
    // 停止TcpServe线程里的事件循环
    void stop();
    

    // // 处理客户端连接请求
    void newconnection(std::unique_ptr<Socket> clientsock);
    // 关闭客户端的连接，在Connection类中回调此函数,并从connections_中删除
    void closeconnection(spConnection conn);
    // 客户端连接出错，在Connection类中回调此函数,并从connections_中删除
    void errorconnection(spConnection conn);
    // 处理客户端发送的消息，在Connection类中回调此函数
    void onmessage(spConnection conn, string &msg);
    // 发送完数据后的处理函数
    void sendcompete(spConnection conn);
    // 处理epoll超时事件
    void epolltimeout(EventLoop *loop);

    void setnewconnectioncb(function<void(spConnection)> newconnection);
    void setcloseconnectioncb(function<void(spConnection)> closeconnection);
    void seterrorconnectioncb(function<void(spConnection)> errorconnection);
    void setonmessagecb(function<void(spConnection, string &)> onmessage);
    void setsendcompetecb(function<void(spConnection)> sendcompete);
    void setepolltimeoutcb(function<void(EventLoop *)> epolltimeout);
    void removeconnection(int fd);
};
